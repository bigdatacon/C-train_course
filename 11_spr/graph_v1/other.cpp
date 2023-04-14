#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

namespace svg {

    struct Point {
        Point() = default;
        Point(double x, double y)
            : x(x)
            , y(y) {
        }
        double x = 0;
        double y = 0;
    };

    struct RenderContext {
        RenderContext(std::ostream& out)
            : out(out) {
        }

        RenderContext(std::ostream& out, int indent_step, int indent = 0)
            : out(out)
            , indent_step(indent_step)
            , indent(indent) {
        }

        RenderContext Indented() const {
            return { out, indent_step, indent + indent_step };
        }

        void RenderIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        std::ostream& out;
        int indent_step = 0;
        int indent = 0;
    };

    class Object {
    public:
        void Render(const RenderContext& context) const;

        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext& context) const = 0;
    };

    class Circle final : public Object {
    public:
        Circle& SetCenter(Point center) {
            center_ = center;
            return *this;
        }

        Circle& SetRadius(double radius) {
            radius_ = radius;
            return *this;
        }

    private:
        void RenderObject(const RenderContext& context) const override;

        Point center_;
        double radius_ = 1.0;
    };

    class Polyline {
    public:
        Polyline& AddPoint(Point point);

    };

    class Text {
    public:
        Text& SetPosition(Point pos);

        Text& SetOffset(Point offset);

        Text& SetFontSize(uint32_t size);

        Text& SetFontFamily(std::string font_family);

        Text& SetFontWeight(std::string font_weight);

        Text& SetData(std::string data);

    };

    class Document {
    public:
        void AddPtr(std::unique_ptr<Object>&& obj);

        void Render(std::ostream& out) const;

    private:
        std::vector<std::unique_ptr<Object>> objects_;
    };

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        RenderObject(context);

        context.out << std::endl;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        context.out << "<circle cx=\"" << center_.x << "\" cy=\"" << center_.y
            << "\" r=\"" << radius_ << "\"/>";
    }

    Polyline& Polyline::AddPoint(Point point) {
        return *this;
    }

    Text& Text::SetPosition(Point pos) {
        return *this;
    }

    Text& Text::SetOffset(Point offset) {
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size) {
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family) {
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight) {
        return *this;
    }

    Text& Text::SetData(std::string data) {
        return *this;
    }

    void Document::AddPtr(std::unique_ptr<Object>&& obj) {
        objects_.push_back(std::move(obj));
    }

    void Document::Render(std::ostream& out) const {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" <<
