#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <variant>
#include <sstream>



namespace svg {

	struct Rgb {
		Rgb() = default;
		Rgb(uint8_t  red, uint8_t  green, uint8_t  blue)
			: red(red)
			, green(green)
			, blue(blue)
		{
		}

		uint8_t red = 0;
		uint8_t green = 0;
		uint8_t blue = 0;
	};

	struct Rgba {
		Rgba() = default;
		Rgba(uint8_t  red, uint8_t  green, uint8_t blue, double  opacity)
			: red(red)
			, green(green)
			, blue(blue)
			, opacity(opacity)
		{
		}

		/*uint8_t red = 215;
		uint8_t  green = 30;
		uint8_t blue = 25;
		double opacity = 0.3;*/

		uint8_t red = 0;
		uint8_t  green = 0;
		uint8_t blue = 0;
		double opacity = 1.0;
	};

	//std::ostream& operator<<(std::ostream& os, const Rgb& rgb); 

	//std::ostream& operator<<(std::ostream& os, const Rgba& rgba);

	//using Color = std::string;
	using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;

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

	struct ColorPrinter {
		std::ostream& os;
		void operator()(std::monostate) const {
			os << std::string("none") /*<< std::endl*/;
		}
		void operator()(const std::string& s) const {
			os << s /*<< std::endl*/;
		}
		void operator()(const Rgb& r) const {
			os << "rgb(" << static_cast<unsigned int>(r.red) << "," << static_cast<unsigned int>(r.green) << "," << static_cast<unsigned int>(r.blue) << ")" /*<< std::endl*/;
		}
		void operator()(const Rgba& r) const {
			os << "rgba(" << static_cast<unsigned int>(r.red) << "," << static_cast<unsigned int>(r.green) << "," << static_cast<unsigned int>(r.blue) << "," << r.opacity << ")" /*<< std::endl*/;
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

		Owner& SetStrokeWidth(double width) {
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

			/*if (auto strPtr = std::get_if<std::string>(&fill_color_)) {
				out << " fill=\""sv << *strPtr << "\""sv;
			}
			else if (auto rgbPtr = std::get_if<Rgb>(&fill_color_)) {
				out << " fill=\""sv << *rgbPtr << "\""sv;
			}
			else if (auto rgbaPtr = std::get_if<Rgba>(&fill_color_)) {
				out << " fill=\""sv << *rgbaPtr << "\""sv;
			}
			if (auto strPtr = std::get_if<std::string>(&stroke_color_)) {
				out << " stroke=\""sv << *strPtr << "\""sv;
			}
			else if (auto rgbPtr = std::get_if<Rgb>(&stroke_color_)) {
				out << " stroke=\""sv << *rgbPtr << "\""sv;
			}
			else if (auto rgbaPtr = std::get_if<Rgba>(&stroke_color_)) {
				out << " stroke=\""sv << *rgbaPtr << "\""sv;
			}*/


			if (fill_color_) {
				//out << "fill=\""sv << *fill_color_ << "\""sv;
				//std::visit(ColorPrinter{}, *fill_color_);
				//out << std::visit(ColorPrinter{ std::cout }, *fill_color_)<< "\""sv;
				out << "fill=\""sv;
				//std::visit(ColorPrinter{ std::cout }, *fill_color_);
				std::visit(ColorPrinter{ out }, *fill_color_);
				out << "\""sv;
			}
			if (stroke_color_) {
				//cout << " stroke=\""sv << *stroke_color_ << "\""sv;
				//std::visit(ColorPrinter{}, *stroke_color_);
				//out << std::visit(ColorPrinter{ std::cout }, *stroke_color_) << "\""sv;
				out << " stroke=\""sv;
				//std::visit(ColorPrinter{ std::cout }, *stroke_color_);
				std::visit(ColorPrinter{ out }, *stroke_color_);
				out << "\""sv;
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
		std::optional<double> stroke_width_;
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

	/*
	class PolylineVec : public  Object, public PathProps<Polyline> {
	public:
		// ��������� ��������� ������� � ������� �����
		PolylineVec& AddPolyline(Polyline polilyne);

	private:
		std::vector<Polyline> polylines_;
		void RenderObject(const RenderContext& context) const override;
	};
	*/



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
		//uint32_t x, y; // ���������� ������� �����
		//uint32_t dx, dy; // �������� ������������ ������� �����

		double x, y; // ���������� ������� �����
		double dx, dy; // �������� ������������ ������� �����


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

