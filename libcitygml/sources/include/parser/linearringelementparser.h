#pragma once

#include <parser/gmlobjectparser.h>

#include <functional>

namespace citygml {

    class LinearRing;

    class LinearRingElementParser : public GMLObjectElementParser {
    public:
        LinearRingElementParser(CityGMLDocumentParser& documentParser, CityGMLFactory& factory, std::shared_ptr<CityGMLLogger> logger, bool interior, std::function<void(LinearRing*)> callback);

        // ElementParser interface
        virtual std::string elementParserName() const override;
        virtual bool handlesElement(const NodeType::XMLNode &node) const override;
    protected:
        // CityGMLElementParser interface
        virtual bool parseElementStartTag(const NodeType::XMLNode& node, Attributes& attributes) override;
        virtual bool parseElementEndTag(const NodeType::XMLNode& node, const std::string& characters) override;
        virtual bool parseChildElementStartTag(const NodeType::XMLNode& node, Attributes& attributes) override;
        virtual bool parseChildElementEndTag(const NodeType::XMLNode& node, const std::string& characters) override;

        // GMLObjectElementParser interface
        virtual Object* getObject() override;

    private:
        LinearRing* m_model;
        std::function<void(LinearRing*)> m_callback;
        bool m_interior;
    };

}
