#ifndef HTTP_LINK_HEADER_H
#define HTTP_LINK_HEADER_H

#include <uriparser/Uri.h>

#include <string>
#include <set>
#include <vector>
#include <sstream>
#include <iostream>

namespace http_link_header {

    namespace uri {

        class Uri {
        public:
            Uri() = default;

            ~Uri() {
                uriFreeUriMembersA(&uri_);
            }

            UriUriA* get_mutable_uri() {
                return &uri_;
            }

            UriUriA* get_uri() const {
                return const_cast<UriUriA*>(&uri_);
            }

            /**
             * Is this URI absolute? (Does it have a "scheme"?)
             */
            bool isAbsolute() const {
                if(!uri_.scheme.first || !uri_.scheme.afterLast)
                    return false;
                return true;
            }

        private:
            UriUriA uri_{};
        };

        /**
         * "Relatively Resolve" the uriToResolve against the baseUri and store the
         * resolvedUri into result.
         *
         * See [RFC3986], Section 5.2.
         *
         * @param baseUri the base URI to resolve against
         * @param uriToResolve the URI to resolve
         * @param result where to store the result
         * @return true is resolution if successful
         */
        bool resolve(const std::string *baseUri, const std::string *uriToResolve, std::string *result) {

            // set up Uri wrapper objects for uriparser library calls

            UriParserStateA state;

            Uri base_uri;
            state.uri = base_uri.get_mutable_uri();
            if (uriParseUriA(&state, baseUri->c_str()) != URI_SUCCESS) {
                return false;
            }
            if(!base_uri.isAbsolute())
                return false;

            Uri relative_uri;
            state.uri = relative_uri.get_mutable_uri();
            if (uriParseUriA(&state, uriToResolve->c_str()) != URI_SUCCESS) {
                return false;
            }

            // resolve the uri

            Uri result_uri;
            if(uriAddBaseUriA(
                    result_uri.get_mutable_uri(),
                    relative_uri.get_uri(),
                    base_uri.get_uri()) != URI_SUCCESS)
                return false;

            // convert uri to string and return

            int chars_required;
            if (uriToStringCharsRequiredA(result_uri.get_uri(),
                                          &chars_required) != URI_SUCCESS) {
                return false;
            }
            std::unique_ptr<char> dest_str(new char[chars_required+1]);
            if (!dest_str) {
                return false;
            }
            int chars_written;
            if (uriToStringA(dest_str.get(), result_uri.get_uri(),
                             chars_required+1, &chars_written) != URI_SUCCESS) {
                return false;
            }
            *result = dest_str.get();

            return true;
        }

    }

    class TargetAttribute {
    public:
        std::string name;
        std::string value;

        bool operator==(const TargetAttribute &rhs) const {
            return name == rhs.name &&
                   value == rhs.value;
        }

        bool operator!=(const TargetAttribute &rhs) const {
            return !(rhs == *this);
        }
    };

    class Link {
    public:
        std::string linkContext;
        std::string linkRelation;
        std::string linkTarget;
        std::vector<TargetAttribute> targetAttributes;

        bool operator==(const Link &rhs) const {
            return linkContext == rhs.linkContext &&
                   linkRelation == rhs.linkRelation &&
                   linkTarget == rhs.linkTarget &&
                   targetAttributes == rhs.targetAttributes;
        }

        bool operator!=(const Link &rhs) const {
            return !(rhs == *this);
        }
    };

    /**
     * Parses a quoted string.
     *
     * Given input, return an unquoted string. input is modified to remove the parsed string.
     *
     * @param input current value of the header string
     * @return unquoted string
     */
    std::string parseQuotedString(std::string &input) {

        // 1. Let output be an empty string.
        std::string output;

        // 2. If the first character of input is not DQUOTE, return output.
        if (input[0] != '"')
            return output;

        // 3. Discard the first character.
        input = input.substr(1);

        // 4. While input has content:
        while(!input.empty()) {

            // 4.1. If the first character is a backslash ("\"):
            if (input[0] == '\\') {

                // 4.1.1. Discard the first character.
                input = input.substr(1);

                // 4.1.2. If there is no more input, return output.
                if(input.empty())
                    return output;

                // 4.1.3. Else, consume the first character and append it to
                // output.
                output.push_back(input[0]);
                input = input.substr(1);
            }

            // 4.2. Else, if the first character is DQUOTE, discard it and return
            // output.
            else if (input[0] == '"') {
                input = input.substr(1);
                return output;
            }

            // 4.3. Else, consume the first character and append it to output.
            else {
                output.push_back(input[0]);
                input = input.substr(1);
            }
        }

        // 5. Return output.
        return output;
    }

    std::vector<TargetAttribute> parseParameters(std::string & input) {

        std::string whitespace = " \t";

        // 1. Let parameters be an empty list
        std::vector<TargetAttribute> parameters;

        // 2. While input has content:
        while(!input.empty()) {

            // 2.1. Consume any leading OWS.
            auto strBegin = input.find_first_not_of(whitespace);
            if(strBegin != std::string::npos)
                input = input.substr(strBegin);

            // 2.2. If the first character is not ";", return parameters.
            if (input[0] != ';')
                return parameters;

            // 2.3. Discard the leading ";" character.
            input = input.substr(1);

            // 2.4. Consume any leading OWS.
            strBegin = input.find_first_not_of(whitespace);
            if(strBegin != std::string::npos)
                input = input.substr(strBegin);

            // 2.5. Consume up to but not including the first BWS, "=", ";", or
            //  "," character, or up to the end of input, and let the result
            //  be parameter_name.
            std::string stopChars = " \t=;,";
            strBegin = input.find_first_of(stopChars);
            std::string parameter_name = input.substr(0, strBegin);
            input = input.substr(strBegin);

            // 2.6.   Consume any leading BWS.
            strBegin = input.find_first_not_of(whitespace);
            if(strBegin != std::string::npos)
                input = input.substr(strBegin);

            std::string parameter_value;

            // 2.7. If the next character is "=":
            if (input[0] == '=') {
                // 2.7.1. Discard the leading "=" character.
                input = input.substr(1);

                // 2.7.2. Consume any leading BWS.
                strBegin = input.find_first_not_of(whitespace);
                if(strBegin != std::string::npos)
                    input = input.substr(strBegin);

                // 2.7.3. If the next character is DQUOTE, let parameter_value be
                //        the result of Parsing a Quoted String (Appendix B.4)
                //        from input (consuming zero or more characters of it).
                if(input[0] == '"') {
                    parameter_value = parseQuotedString(input);
                }

                // 2.7.4. Else, consume the contents up to but not including the
                //        first ";" or "," character, or up to the end of input,
                //        and let the results be parameter_value.
                else {
                    stopChars = ";,";
                    strBegin = input.find_first_not_of(stopChars);
                    if(strBegin != std::string::npos) {
                        parameter_value = input.substr(strBegin - 1);
                        input = input.substr(strBegin);
                    }
                }

                // 2.7.5. If the last character of parameter_name is an asterisk
                //        ("*"), decode parameter_value according to [RFC8187].
                //        Continue processing input if an unrecoverable error is
                //        encountered.
                if(parameter_name[parameter_name.size()-1] == '*') {
                    // todo ...
                }
            }
            else {
                // 2.8. Else:
                // 2.8.1. Let parameter_value be an empty string.
                parameter_value = "";
            }

            // 2.9. Case-normalise parameter_name to lowercase.
            std::transform(parameter_name.begin(), parameter_name.end(),
                           parameter_name.begin(), &::tolower);

            // 2.10. Append (parameter_name, parameter_value) to parameters.
            parameters.push_back(TargetAttribute{parameter_name, parameter_value});

            // 2.11. Consume any leading OWS.
            strBegin = input.find_first_not_of(whitespace);
            if(strBegin != std::string::npos)
                input = input.substr(strBegin);

            // 2.12. If the next character is "," or the end of input, stop
            //       processing input and return parameters.
            if(input.empty())
                return parameters;
            if(input[0] == ',') {
                input = input.substr(1);
                return parameters;
            }
        }

        return parameters;
    }

    /**
     * Parses zero or more comma-separated link-values from a Link header field
     *
     * @param linkHeaderField string containing the value of a Link header field
     *
     * @return vector of zero or more Link objects
     */
    std::vector<Link> parse(const std::string& linkHeaderField, const std::string &baseUri = "") {

        std::vector<Link> links;

        std::string whitespace = " \t";

        std::string field_value = linkHeaderField;

        while(!field_value.empty()) {

            // 1. Consume any leading OWS
            auto strBegin = field_value.find_first_not_of(whitespace);
            if(strBegin != std::string::npos)
                field_value = field_value.substr(strBegin);

            // 2. If the first character is not "<", return links.
            if(field_value[0] != '<')
                return links;

            // 3. Discard the first character ("<").
            // 4. Consume up to but not including the first ">" character or
            //    end of field_value and let the result be target_string.
            auto strEnd = field_value.find_first_of('>');
            std::string target_string = field_value.substr(1, strEnd-1);
            field_value = field_value.substr(strEnd);

            // 5. If the next character is not ">", return links.
            if(field_value[0] != '>')
                return links;

            // 6. Discard the leading ">" character.
            field_value = field_value.substr(1);

            // 7. Let link_parameters be the result of Parsing Parameters
            //    (Appendix B.3) from field_value (consuming zero or more
            //    characters of it).
            std::vector<TargetAttribute> link_parameters =
                    parseParameters(field_value);

            // 8. Let target_uri be the result of relatively resolving (as per
            //   [RFC3986], Section 5.2) target_string.  Note that any base
            //   URI carried in the payload body is NOT used.
            std::string target_uri;
            if(!uri::resolve(&baseUri, &target_string, &target_uri))
                target_uri = target_string;

            // 9. Let relations_string be the second item of the first tuple
            //    of link_parameters whose first item matches the string "rel"
            //    or the empty string ("") if it is not present.
            std::string relations_string;
            for(const auto& e : link_parameters) {
                if(e.name == "rel") {
                    relations_string = e.value;
                    continue;
                }
            }

            // 10. Split relations_string on RWS (removing it in the process)
            //     into a list of string relation_types.
            std::vector<std::string> relation_types;
            if(relations_string.find_first_of(whitespace) != std::string::npos) {
                while ((strEnd = relations_string.find_first_of(whitespace)) != std::string::npos) {
                    std::string relation_type = relations_string.substr(0, strEnd);
                    relations_string = relations_string.substr(strEnd + 1);
                    relation_types.push_back(relation_type);
                }
                if (!relations_string.empty())
                    relation_types.push_back(relations_string);
            }
            else
                relation_types.push_back(relations_string);

            // 11. Let context_string be the second item of the first tuple of
            //     link_parameters whose first item matches the string
            //     "anchor".  If it is not present, context_string is the URL
            //     of the representation carrying the Link header [RFC7231],
            //     Section 3.1.4.1, serialised as a URI.  Where the URL is
            //     anonymous, context_string is null.
            std::string context_string;
            for(const auto& e : link_parameters) {
                if(e.name == "anchor") {
                    context_string = e.value;
                    continue;
                }
            }

            // 12. Let context_uri be the result of relatively resolving (as
            //     per [RFC3986], Section 5.2) context_string, unless
            //     context_string is null, in which case context is null.  Note
            //     that any base URI carried in the payload body is NOT used.
            std::string context_uri;
            if(!uri::resolve(&baseUri, &context_string, &context_uri))
                context_uri = context_string;

            // 13. Let target_attributes be an empty list.
            std::vector<TargetAttribute> target_attributes;

            // 14. For each tuple (param_name, param_value) of link_parameters:
            for(const auto& param : link_parameters) {

                // 14.1. If param_name matches "rel" or "anchor", skip this tuple.
                if(param.name == "rel" || param.name == "anchor")
                    continue;

                // 14.2. If param_name matches "media", "title", "title*", or
                //       "type" and target_attributes already contains a tuple
                //       whose first element matches the value of param_name,
                //       skip this tuple.
                if(param.name == "media" ||
                   param.name == "title" ||
                   param.name == "title*" ||
                   param.name == "type") {
                    for(const auto& a : target_attributes)
                        if(a.name == param.name)
                            continue;
                }

                // 14.3. Append (param_name, param_value) to target_attributes.
                target_attributes.push_back(param);
            }

            // 15. Let star_param_names be the set of param_names in the
            //     (param_name, param_value) tuples of target_attributes where
            //     the last character of param_name is an asterisk ("*").
            std::vector<std::string> star_param_names;
            for(const auto& param : target_attributes)
                if(param.name[param.name.size()-1] == '*')
                    star_param_names.push_back(param.name);

            // 16. For each star_param_name in star_param_names:
            for(const auto& star_param_name : star_param_names) {

                // 16.1. Let base_param_name be star_param_name with the last
                //       character removed.
                std::string base_param_name =
                        star_param_name.substr(0, star_param_name.size()-1);

                // 16.2. If the implementation does not choose to support an
                //       internationalised form of a parameter named
                //       base_param_name for any reason (including, but not
                //       limited to, it being prohibited by the parameter’s
                //       specification), remove all tuples from target_attributes
                //       whose first member is star_param_name, and skip to the
                //       next star_param_name.
                // todo...

                // 16.3. Remove all tuples from target_attributes whose first
                //       member is base_param_name.
                target_attributes.erase(
                        std::remove_if(target_attributes.begin(), target_attributes.end(),
                                       [&](const TargetAttribute &x) { return x.name == base_param_name; }),
                        target_attributes.end());

                // 16.4. Change the first member of all tuples in target_attributes
                //       whose first member is star_param_name to base_param_name.
                for(auto& param : target_attributes) {
                    if(param.name == star_param_name)
                        param.name = base_param_name;
                }
            }

            // 17. For each relation_type in relation_types:
            for(auto relation_type : relation_types) {
                // 17.1. Case-normalise relation_type to lowercase.
                std::transform(relation_type.begin(), relation_type.end(),
                               relation_type.begin(), &::tolower);
                // 17.2. Append a link object to links with the target
                //       target_uri, relation type of relation_type, context of
                //       context_uri, and target attributes target_attributes.
                links.push_back(Link{context_uri, relation_type, target_uri, target_attributes});
            }
        }

        return links;
    }

    /**
     * Parse the Link header fields that a HTTP header set contains
     *
     * @param headers vector of HTTP header strings
     *
     * @return vector of zero or more Link objects
     */
    std::vector<Link> parse(const std::vector<std::string>& headers, const std::string &baseUri = "") {
        std::vector<Link> links;

        if(headers.empty())
            return links;

        for(const auto& header : headers) {
            std::vector<Link> headerLinks = parse(header, baseUri);
            links.insert(links.end(), headerLinks.begin(), headerLinks.end());
        }

        return links;
    }


}


#endif //HTTP_LINK_HEADER_H
