#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
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

    /*
     * ��������������� ���������, �������� �������� ��� ������ SVG-��������� � ���������.
     * ������ ������ �� ����� ������, ������� �������� � ��� ������� ��� ������ ��������
     */
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

    /*
     * ����������� ������� ����� Object ������ ��� ���������������� ��������
     * ���������� ����� SVG-���������
     * ��������� ������� "��������� �����" ��� ������ ����������� ����
     */
    class Object {
    public:
        void Render(const RenderContext& context) const;

        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext& context) const = 0;
    };


    class Circle final : public Object {
    public:
        Circle& SetCenter(Point center);
        Circle& SetRadius(double radius);

    private:
        void RenderObject(const RenderContext& context) const override;

        Point center_;
        double radius_ = 1.0;
    };

    class Polyline : public  Object {
    public:
        // ��������� ��������� ������� � ������� �����
        Polyline& AddPoint(Point point);



    private:
        std::vector<Point> points_;
        void RenderObject(const RenderContext& context) const override;
    };


    class Text : public Object {
    public:
        // ����� ���������� ������� ����� (�������� x � y)
        Text& SetPosition(Point pos);

        // ����� �������� ������������ ������� ����� (�������� dx, dy)
        Text& SetOffset(Point offset);

        // ����� ������� ������ (������� font-size)
        Text& SetFontSize(uint32_t size);

        // ����� �������� ������ (������� font-family)
        Text& SetFontFamily(std::string font_family);

        // ����� ������� ������ (������� font-weight)
        Text& SetFontWeight(std::string font_weight);

        // ����� ��������� ���������� ������� (������������ ������ ���� text)
        Text& SetData(std::string data);

        // ������ ������ � ������, ����������� ��� ���������� �������� <text>
    private:
        uint32_t x, y; // ���������� ������� �����
        uint32_t dx, dy; // �������� ������������ ������� �����
        uint32_t font_size = 1; // ������ ������
        std::string font_family_name; // �������� ������
        std::string font_weight_type; // ������� ������
        std::string text_data; // ��������� ���������� �������

        void RenderObject(const RenderContext& context) const override;

    };


    class ObjectContainer {
    public:
        template <typename Obj>
        void Add(Obj obj) {
            objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
        }
        // ��������� � svg-�������� ������-��������� svg::Object
        virtual void AddPtr(std::unique_ptr<Object>&& obj) const = 0;

        virtual ~ObjectContainer() = default;

    private:
        std::vector<std::unique_ptr<Object>> objects_;
    };

    // ��������� Drawable ����� �������, ������� ����� ���������� � ������� ObjectContainer
    class Drawable {
    public:
        virtual void Draw(ObjectContainer& o) const = 0;
        virtual ~Drawable() = default;
    };

    class Document : public ObjectContainer {
    public:
        /*template <typename Obj>
        void Add(Obj obj) {
            objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
        }*/
        // ��������� � svg-�������� ������-��������� svg::Object
        void AddPtr(std::unique_ptr<Object>&& obj) const override;

        // ������� � ostream svg-������������� ���������
        void Render(std::ostream& out) const;

        virtual ~Document() = default;

    private:
        std::vector<std::unique_ptr<Object>> objects_;
    };


}  // namespace svg
