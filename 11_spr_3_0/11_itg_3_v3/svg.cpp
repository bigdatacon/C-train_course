#include "svg.h"
#include <iostream>
#include <sstream>
namespace svg {

	using namespace std::literals;

	std::ostream& operator<<(std::ostream& os, const StrokeLineCap& cap) {
		switch (cap) {
		case StrokeLineCap::BUTT: os << "butt"; break;
		case StrokeLineCap::ROUND: os << "round"; break;
		case StrokeLineCap::SQUARE: os << "square"; break;
		}
		return os;
	}

	std::ostream& operator<<(std::ostream& os, const StrokeLineJoin& join) {
		switch (join) {
		case StrokeLineJoin::ARCS: os << "arcs"; break;
		case StrokeLineJoin::BEVEL: os << "bevel"; break;
		case StrokeLineJoin::MITER: os << "miter"; break;
		case StrokeLineJoin::MITER_CLIP: os << "miter-clip"; break;
		case StrokeLineJoin::ROUND: os << "round"; break;
		}
		return os;
	}

	/*std::ostream& operator<<(std::ostream& os, const Rgb& rgb) {
		os << "rgb(" << static_cast<unsigned int>(rgb.red_) << ","
			<< static_cast<unsigned int>(rgb.green_) << ","
			<< static_cast<unsigned int>(rgb.blue_) << ")";
		return os;
	}

	std::ostream& operator<<(std::ostream& os, const Rgba& rgba) {
		os << "rgb(" << static_cast<unsigned int>(rgba.red_) << ","
			<< static_cast<unsigned int>(rgba.green_) << ","
			<< static_cast<unsigned int>(rgba.blue_) << ","
			<< static_cast<unsigned int>(rgba.opacity_) << ")";
		return os;
	}*/

	std::ostream& operator<<(std::ostream& os, const Color& color) {
		std::ostringstream out;
		std::visit(ColorPrinter{ out }, color);
		os << out.str();
		return os;
	}


	void Object::Render(const RenderContext& context) const {
		context.RenderIndent();

		// Делегируем вывод тега своим подклассам
		RenderObject(context);

		context.out << std::endl;
	}

	//------------------PathProps----------------------------
	/*void PathProps::RenderAttrs(std::ostream& out) const {
		using namespace std::literals;
		if (fill_color_) {
			out << " fill=\""sv << *fill_color_ << "\""sv;
		}
		if (stroke_color_) {
			out << " stroke=\""sv << *stroke_color_ << "\""sv;
		}
	}*/

	// ---------- Circle ------------------

	Circle& Circle::SetCenter(Point center) {
		center_ = center;
		return *this;
	}

	Circle& Circle::SetRadius(double radius) {
		radius_ = radius;
		return *this;
	}

	void Circle::RenderObject(const RenderContext& context) const {
		auto& out = context.out;
		out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
		out << "r=\""sv << radius_ << "\" "sv;
		RenderAttrs(context.out);
		out << "/>"sv;
	}

	//--------------------Polyline

		// Добавляет очередную вершину к ломаной линии
	Polyline& Polyline::AddPoint(Point point) {
		points_.push_back(point);
		return *this;
	}


	// Отрисовывает ломаную линию
	void Polyline::RenderObject(const RenderContext& context) const {
		auto& out = context.out;
		out << "<polyline points=\"";
		for (auto it = points_.begin(); it != points_.end(); ++it) {
			out << it->x << "," << it->y;
			if (it != points_.end() - 1) {
				out << " ";
			}
		}
		//std::cout << "\"";
		/*out << " />";
		RenderAttrs(context.out);
		out << "/>";*/

		out << "\" ";

		RenderAttrs(out);
		//out << ' ';
		out << "/>"sv;

	}

	/*
	//--------------------PolylineVec
			// Добавляет очередную вершину к ломаной линии
	PolylineVec& PolylineVec::AddPolyline(Polyline polilyne) {
		polylines_.push_back(polilyne);
		return *this;
	}


	// Отрисовывает ломаную линию
	void PolylineVec::RenderObject(const RenderContext& context) const {
		auto& out = context.out;
		out << "<polyline points=\"";
		for (auto polyline : polylines_) {
			for (auto point : polyline) {
				out << point->x << "," << point->y;
				if (polyline[point] != polyline.end() - 1) {
					out << " ";
				}
			}
			out << "\" ";
			RenderAttrs(out);
			out << "/>"sv;
		}



	}
	*/





	/*
	void Polyline::RenderObject(const RenderContext& context) const  {
		auto& out = context.out;
		out << "<polyline points=\"";
		for (const Point& p : points_) {
			out << p.x << "," << p.y << " ";
		}
		out << "\" />";
	}*/


	//--------------------------------------Text
		// Задаёт координаты опорной точки (атрибуты x и y)
	Text& Text::SetPosition(Point pos) {
		x = pos.x;
		y = pos.y;
		return *this;
	}

	// Задаёт смещение относительно опорной точки (атрибуты dx, dy)
	Text& Text::SetOffset(Point offset) {
		dx = offset.x;
		dy = offset.y;
		return *this;
	}

	// Задаёт размеры шрифта (атрибут font-size)
	Text& Text::SetFontSize(uint32_t size) {
		font_size = size;
		return *this;
	}

	// Задаёт название шрифта (атрибут font-family)
	Text& Text::SetFontFamily(std::string font_family) {
		font_family_name = font_family;
		return *this;
	}

	// Задаёт толщину шрифта (атрибут font-weight)
	Text& Text::SetFontWeight(std::string font_weight) {
		font_weight_type = font_weight;
		return *this;
	}

	// Задаёт текстовое содержимое объекта (отображается внутри тега text)
	Text& Text::SetData(std::string data) {
		text_data = data;
		return *this;
	}

	//Text& SetStrokeLineJoin(StrokeLineJoin);
	//Text& SetStrokeLineCap(StrokeLineCap);
	/*Text& Text::SetStrokeWidth(uint32_t width) {
		width_ = width;
		return *this;
	};*/


	void Text::RenderObject(const RenderContext& context) const {
		auto& out = context.out;
		out << "<text ";
		RenderAttrs(context.out);
		out << " x=\"" << x << "\" y=\"" << y << "\"";
		//out << "<text x=\"" << x << "\" y=\"" << y << "\"";



		out << " dx=\"" << dx << "\" dy=\"" << dy << "\"";

		out << " font-size=\"" << font_size << "\"";
		if (!font_family_name.empty()) {
			out << " font-family=\"" << font_family_name << "\"";
		}
		if (!font_weight_type.empty()) {
			out << " font-weight=\"" << font_weight_type << "\"";
		}

		out << ">" << text_data << "</text>";

	}


	//-----------------ObjectContainer
		// Добавляет в svg-документ объект-наследник svg::Object
	/*void ObjectContainer::AddPtr(std::unique_ptr<Object>&& obj) {
		objects_.emplace_back(std::move(obj));
	}*/

	// Интерфейс Drawable задаёт объекты, которые можно нарисовать с помощью Graphics
	//Drawable----------------------------------------------------------
	//virtual void Drawable::Draw(ObjectContainer& o) const = 0;


	//---------------------------Document


		// Добавляет в svg-документ объект-наследник svg::Object
	void Document::AddPtr(std::unique_ptr<Object>&& obj) {
		objects_.emplace_back(std::move(obj));
	}

	// Выводит в ostream svg-представление документа
	void Document::Render(std::ostream& out) const {
		out << R"(<?xml version="1.0" encoding="UTF-8" ?>)" << std::endl;
		out << R"(<svg xmlns="http://www.w3.org/2000/svg" version="1.1">)" << std::endl;
		RenderContext context(out, 2);
		for (const auto& obj : objects_) {
			obj->Render(context.Indented());
		}
		out << "</svg>" << std::endl;

	}






}  // namespace svg

