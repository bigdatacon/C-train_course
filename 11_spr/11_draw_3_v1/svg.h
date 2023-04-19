#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <optional>



namespace svg {

	using Color = std::string;

	// Объявив в заголовочном файле константу со спецификатором inline,
	// мы сделаем так, что она будет одной на все единицы трансляции,
	// которые подключают этот заголовок.
	// В противном случае каждая единица трансляции будет использовать свою копию этой константы
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
	 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
	 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
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
	 * Абстрактный базовый класс Object служит для унифицированного хранения
	 * конкретных тегов SVG-документа
	 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
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
				out << " fill=\""sv << *fill_color_ << "\""sv;
			}
			if (stroke_color_) {
				out << " stroke=\""sv << *stroke_color_ << "\""sv;
			}
		}

	private:
		Owner& AsOwner() {
			// static_cast безопасно преобразует *this к Owner&,
			// если класс Owner — наследник PathProps
			return static_cast<Owner&>(*this);
		}

		std::optional<Color> fill_color_;
		std::optional<Color> stroke_color_;
		std::optional<StrokeLineJoin> stroke_line_join_;
		std::optional<StrokeLineCap> stroke_line_cap_;
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
		// Добавляет очередную вершину к ломаной линии
		Polyline& AddPoint(Point point);

	private:
		std::vector<Point> points_;
		void RenderObject(const RenderContext& context) const override;
	};


	class Text : public Object, public PathProps<Text> {
	public:
		// Задаёт координаты опорной точки (атрибуты x и y)
		Text& SetPosition(Point pos);

		// Задаёт смещение относительно опорной точки (атрибуты dx, dy)
		Text& SetOffset(Point offset);

		// Задаёт размеры шрифта (атрибут font-size)
		Text& SetFontSize(uint32_t size);

		// Задаёт название шрифта (атрибут font-family)
		Text& SetFontFamily(std::string font_family);

		// Задаёт толщину шрифта (атрибут font-weight)
		Text& SetFontWeight(std::string font_weight);

		// Задаёт текстовое содержимое объекта (отображается внутри тега text)
		Text& SetData(std::string data);

		// Прочие данные и методы, необходимые для реализации элемента <text>
		//Text& SetStrokeLineJoin(StrokeLineJoin);
		//Text& SetStrokeLineCap(StrokeLineCap);
		Text& SetStrokeWidth(uint32_t width);


	private:
		uint32_t x, y; // Координаты опорной точки
		uint32_t dx, dy; // Смещение относительно опорной точки
		uint32_t font_size = 1; // Размер шрифта
		std::string font_family_name; // Название шрифта
		std::string font_weight_type; // Толщина шрифта
		std::string text_data; // Текстовое содержимое объекта


		uint32_t width_;

		void RenderObject(const RenderContext& context) const override;

	};


	class ObjectContainer {
	public:
		template <typename Obj>
		void Add(Obj obj) {
			AddPtr(std::make_unique<Obj>(std::move(obj)));
		}
		// Добавляет в svg-документ объект-наследник svg::Object
		virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;

		virtual ~ObjectContainer() = default;
	};

	// Интерфейс Drawable задаёт объекты, которые можно нарисовать с помощью ObjectContainer
	class Drawable {
	public:
		virtual void Draw(ObjectContainer& o) const = 0;
		virtual ~Drawable() = default;
	};

	class Document : public ObjectContainer {
	public:
		// Добавляет в svg-документ объект-наследник svg::Object
		void AddPtr(std::unique_ptr<Object>&& obj) override;

		// Выводит в ostream svg-представление документа
		void Render(std::ostream& out) const;

		virtual ~Document() = default;

	private:
		std::vector<std::unique_ptr<Object>> objects_;
	};


}  // namespace svg
