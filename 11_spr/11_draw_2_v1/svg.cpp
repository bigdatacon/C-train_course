#include "svg.h"

namespace svg {

	using namespace std::literals;

	void Object::Render(const RenderContext& context) const {
		context.RenderIndent();

		// ���������� ����� ���� ����� ����������
		RenderObject(context);

		context.out << std::endl;
	}

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
		out << "/>"sv;
	}

	//--------------------Polyline

		// ��������� ��������� ������� � ������� �����
	Polyline& Polyline::AddPoint(Point point) {
		points_.push_back(point);
		return *this;
	}


	// ������������ ������� �����
	void Polyline::RenderObject(const RenderContext& context) const {
		auto& out = context.out;
		out << "<polyline points=\"";
		for (auto it = points_.begin(); it != points_.end(); ++it) {
			out << it->x << "," << it->y;
			if (it != points_.end() - 1) {
				out << " ";
			}
		}
		out << "\" />";
	}


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
		// ����� ���������� ������� ����� (�������� x � y)
	Text& Text::SetPosition(Point pos) {
		x = pos.x;
		y = pos.y;
		return *this;
	}

	// ����� �������� ������������ ������� ����� (�������� dx, dy)
	Text& Text::SetOffset(Point offset) {
		dx = offset.x;
		dy = offset.y;
		return *this;
	}

	// ����� ������� ������ (������� font-size)
	Text& Text::SetFontSize(uint32_t size) {
		font_size = size;
		return *this;
	}

	// ����� �������� ������ (������� font-family)
	Text& Text::SetFontFamily(std::string font_family) {
		font_family_name = font_family;
		return *this;
	}

	// ����� ������� ������ (������� font-weight)
	Text& Text::SetFontWeight(std::string font_weight) {
		font_weight_type = font_weight;
		return *this;
	}

	// ����� ��������� ���������� ������� (������������ ������ ���� text)
	Text& Text::SetData(std::string data) {
		text_data = data;
		return *this;
	}


	void Text::RenderObject(const RenderContext& context) const {
		auto& out = context.out;
		out << "<text x=\"" << x << "\" y=\"" << y << "\"";

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
		// ��������� � svg-�������� ������-��������� svg::Object
	/*void ObjectContainer::AddPtr(std::unique_ptr<Object>&& obj) {
		objects_.emplace_back(std::move(obj));
	}*/

	// ��������� Drawable ����� �������, ������� ����� ���������� � ������� Graphics 
	//Drawable----------------------------------------------------------
	//virtual void Drawable::Draw(ObjectContainer& o) const = 0;


	//---------------------------Document


		// ��������� � svg-�������� ������-��������� svg::Object
	void Document::AddPtr(std::unique_ptr<Object>&& obj) const {
		objects_.emplace_back(std::move(obj));
	}

	// ������� � ostream svg-������������� ���������
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