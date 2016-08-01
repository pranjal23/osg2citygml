#pragma once

#include <parser/citygmlelementparser.h>

#include <functional>
#include <string>

namespace citygml {

    class GeoreferencedTexture;

    class GeoReferencedTextureElementParser : public CityGMLElementParser {
    public:
        GeoReferencedTextureElementParser(CityGMLDocumentParser& documentParser, CityGMLFactory& factory, std::shared_ptr<CityGMLLogger> logger, std::function<void(std::shared_ptr<GeoreferencedTexture>)> callback);

        // ElementParser interface
        virtual std::string elementParserName() const override;
        virtual bool handlesElement(const NodeType::XMLNode &node) const override;

    protected:
        // CityGMLElementParser interface
        virtual bool parseElementStartTag(const NodeType::XMLNode& node, Attributes& attributes) override;
        virtual bool parseElementEndTag(const NodeType::XMLNode& node, const std::string& characters) override;
        virtual bool parseChildElementStartTag(const NodeType::XMLNode& node, Attributes& attributes) override;
        virtual bool parseChildElementEndTag(const NodeType::XMLNode& node, const std::string& characters) override;

    private:
        std::function<void(std::shared_ptr<GeoreferencedTexture>)> m_callback;
    };

}
