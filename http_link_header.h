#ifndef HTTP_LINK_HEADER_H
#define HTTP_LINK_HEADER_H

#include <string>
#include <set>
#include <vector>
#include <sstream>

namespace http_link_header {

    namespace detail {

        std::vector<std::string> split(const std::string& s, char delimiter)
        {
            std::vector<std::string> tokens;
            std::string token;
            std::istringstream stream(s);
            while (std::getline(stream, token, delimiter))
                tokens.push_back(token);
            return tokens;
        }

    }

    class TargetAttribute {
    public:
        std::string name;
        std::string value;
    };

    class Link {
    public:
        std::string linkContext;
        std::vector<std::string> linkRelations;
        std::string linkTarget;
        std::vector<TargetAttribute> targetAttributes;
    };

    /**
     * Parses zero or more comma-separated link-values from a Link header field
     *
     * @param linkFieldValue string containing the value of a Link header field
     *
     * @return vector of zero or more Link objects
     */
    std::vector<Link> parse(const std::string& linkFieldValue) {
        std::vector<Link> links;

        if(linkFieldValue.empty())
            return links;

        // split up linkFieldValue string into multiple header strings if necessary

        if(linkFieldValue.find("Link: ") != 0)
            return links;

        for(auto s : detail::split(linkFieldValue, ',')) {
            Link link{};
            links.push_back(link);
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
    std::vector<Link> parse(const std::vector<std::string>& headers) {
        std::vector<Link> links;

        if(headers.empty())
            return links;

        for(const auto& header : headers) {
            if (header.find("Link: ") != 0)
                continue;
            std::vector<Link> headerLinks = parse(header);
            links.insert(links.end(), headerLinks.begin(), headerLinks.end());
        }

        return links;
    }


}


#endif //HTTP_LINK_HEADER_H
