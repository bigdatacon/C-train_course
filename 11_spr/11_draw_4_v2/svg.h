#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <variant>



namespace svg {

	struct Rgb {
		Rgb() = default;
		Rgb(uint32_t red, uint32_t green, uint32_t blue)
			: red_(red)
			, green_(green) 
			, blue_(blue)
		{
		}

		uint32_t red_ = 215;
		uint32_t green_ = 30;
		uint32_t blue_ = 25;
	};

	struct Rgba {
		Rgba() = default;
		Rgba(uint32_t red, uint32_t green, uint32_t blue, uint32_t opacity)
			: red_(red)
			, green_(green)
			, blue_(blue)
			, opacity_(opacity)
		{
		}

		uint32_t red_ = 215;
		uint32_t green_ = 30;
		uint32_t blue_ = 25;
		uint32_t opacity_ = 0.3;
	};

	std::ostream& operator<<(std::ostream& os, const Rgb& rgb); 

	std::ostream& operator<<(std::ostream& os, const Rgba& rgba);

	//using Color = std::string;
	using Color = std::variant<std::monostate, std::string, Rgb , Rgba>;

	// ������� � ������������ ����� ��������� �� �������������� inline,
	// �� ������� ���, ��� ��� ����� ����� �� ��� ������� ����������,
	// ������� ���������� ���� ���������.
	// � ��������� ������ ������ ������� ���������� ����� ������������ ���� ����� ���� ���������
	inline const Color NoneColor{ "none" };

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

	enum class StrokeLineCap {
		BUTT,
		ROUND,
		SQUARE,
	};

	enum class StrokeLineJoin {
		ARCS,
		BEVEL,
		MITER,
		MITER_CLIP,
		ROUND,
	};

	std::ostream& operator<<(std::ostream& os, const StrokeLineCap& cap);

	std::ostream& operator<<(std::ostream& os, const StrokeLineJoin& join);

	/*struct ColorPrinter {
		ostream& os;
		void operator()(std::monostate) const {
			os << std::string("none") << std::endl;
		}
		void operator()(const std::string& s) const {
			os << s << std::endl;
		}
		void operator()(const Rgb& r) const {
			os << "RGB(" << r.red_ << "," << r.green_ << "," << r.blue_ << ")" << std::endl;
		}
		void operator()(const Rgba& r) const {
			os << "RGBA(" << r.red_ << "," << r.green_ << "," << r.blue_ << "," << r.opacity_ << ")" << std::endl;
		}
	};*/


	/*struct ColorPrinter {
		void operator()(std::monostate) const {
			std::cout << std::string("none")  << std::endl;
		}
		void operator()(std::string s) const {
			std::cout << s << std::endl;
		}
		void operator()(Rgb r) const {
			std::cout << "RGB(" << r.red_ << "," << r.green_ << "," << r.blue_ << ")" << std::endl;
		}
		void operator()(Rgba r) const {
			std::cout << "RGBA(" << r.red_ << "," << r.green_ << "," << r.blue_ << "," << r.opacity_ << ")" << std::endl;
		}
	};*/

	struct ColorPrinter {
		std::ostream& out;
		void operator()(std::monostate) const {
			out << "none";
		}
		void operator()(std::string color) const {
			out << color;
		}
		void operator()(Rgb rgb) const {
			out << rgb;
		}
		void operator()(Rgba rgba) const {
			out << rgba;
		}
	};

	std::ostream& operator<<(std::ostream& os, const Color& color);




	template <typename Owner>
	class PathProps {
	public:
		Owner& SetFillColor(Color color) {
			fill_color_ = std::move(color);
			return AsOwner();
		}
		Owner& SetStrokeColor(Color color) {
			stroke_color_ = std::move(color);
			return AsOwner();
		}

		Owner& SetStrokeWidth(int width) {
			stroke_width_ = width;
			return AsOwner();
		}

		Owner& SetStrokeLineJoin(StrokeLineJoin Strok) {
			stroke_line_join_ = std::move(Strok);
			return AsOwner();
		}

		Owner& SetStrokeLineCap(StrokeLineCap Strok) {
			stroke_line_cap_ = std::move(Strok);
			return AsOwner();
		}


	protected:
		~PathProps() = default;

		void RenderAttrs(std::ostream& out) const {
			using namespace std::literals;

			if (fill_color_) {
				//out << "fill=\""sv << *fill_color_ << "\""sv;
				std::visit(ColorPrinter{}, *fill_color_);
			}
			if (stroke_color_) {
				//cout << " stroke=\""sv << *stroke_color_ << "\""sv;
				std::visit(ColorPrinter{}, *stroke_color_);
			}
			if (stroke_width_) {
				out << " stroke-width=\""sv << *stroke_width_ << "\""sv;
			}
			if (stroke_line_cap_) {
				out << " stroke-linecap=\""sv << *stroke_line_cap_ << "\""sv;
			}
			if (stroke_line_join_) {
				out << " stroke-linejoin=\""sv << *stroke_line_join_ << "\""sv;
			}

		}

	private:
		Owner& AsOwner() {
			// static_cast ��������� ����������� *this � Owner&,
			// ���� ����� Owner � ��������� PathProps
			return static_cast<Owner&>(*this);
		}

		std::optional<Color> fill_color_;
		std::optional<Color> stroke_color_;
		std::optional<StrokeLineJoin> stroke_line_join_;
		std::optional<StrokeLineCap> stroke_line_cap_;
		std::optional<int> stroke_width_;
	};


	class Circle final : public Object, public PathProps<Circle> {
	public:
		Circle& SetCenter(Point center);
		Circle& SetRadius(double radius);

	private:
		void RenderObject(const RenderContext& context) const override;

		Point center_;
		double radius_ = 1.0;
	};


	class Polyline : public  Object, public PathProps<Polyline> {
	public:
		// ��������� ��������� ������� � ������� �����
		Polyline& AddPoint(Point point);

	private:
		std::vector<Point> points_;
		void RenderObject(const RenderContext& context) const override;
	};


	class Text : public Object, public PathProps<Text> {
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
		//Text& SetStrokeLineJoin(StrokeLineJoin);
		//Text& SetStrokeLineCap(StrokeLineCap);
		//Text& SetStrokeWidth(uint32_t width);


	private:
		uint32_t x, y; // ���������� ������� �����
		uint32_t dx, dy; // �������� ������������ ������� �����
		uint32_t font_size = 1; // ������ ������
		std::string font_family_name; // �������� ������
		std::string font_weight_type; // ������� ������
		std::string text_data; // ��������� ���������� �������


		uint32_t width_;

		void RenderObject(const RenderContext& context) const override;

	};


	class ObjectContainer {
	public:
		template <typename Obj>
		void Add(Obj obj) {
			AddPtr(std::make_unique<Obj>(std::move(obj)));
		}
		// ��������� � svg-�������� ������-��������� svg::Object
		virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;

		virtual ~ObjectContainer() = default;
	};

	// ��������� Drawable ����� �������, ������� ����� ���������� � ������� ObjectContainer
	class Drawable {
	public:
		virtual void Draw(ObjectContainer& o) const = 0;
		virtual ~Drawable() = default;
	};

	class Document : public ObjectContainer {
	public:
		// ��������� � svg-�������� ������-��������� svg::Object
		void AddPtr(std::unique_ptr<Object>&& obj) override;

		// ������� � ostream svg-������������� ���������
		void Render(std::ostream& out) const;

		virtual ~Document() = default;

	private:
		std::vector<std::unique_ptr<Object>> objects_;
	};


}  // namespace svg

