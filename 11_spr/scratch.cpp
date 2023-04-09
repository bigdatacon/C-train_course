
struct Size {
    int width = 0;
    int height = 0;
};

struct Point {
    int x = 0;
    int y = 0;
};
using Image = std::vector<std::string>;

inline Size GetImageSize(const Image& image) {
    const int width = image.empty() ? 0 : static_cast<int>(image[0].size());
    const int height = static_cast<int>(image.size());
    return { width, height };
}

// Проверяет, содержится ли заданная точка в эллипсе заданного размера
// Считается, что эллипс вписан в прямоугольник с вершинами в точках (0, 0) и
// (size.width, size.height)
inline bool IsPointInEllipse(Point p, Size size) {
    // Нормируем координаты точки в диапазон (-1, 1)
    // Сдвиг на 0.5 нужен, чтобы считать расстояние до центра пикселя, так
    // получается более красивая форма
    double x = (p.x + 0.5) / (size.width / 2.0) - 1;
    double y = (p.y + 0.5) / (size.height / 2.0) - 1;
    // Проверяем, лежит ли точка в единичном круге
    return x * x + y * y <= 1;
}

class Texture {
public:
    explicit Texture(Image image)
        : image_(std::move(image)) {
    }

    Size GetSize() const {
        // Заглушка. Реализуйте метод самостоятельно
        return GetImageSize(image_);
    }


    char GetPixelColor(Point p) const {
        const auto [x, y] = p;
        const auto [width, height] = GetSize();
        if (x < 0 || y < 0 || x >= width || y >= height) {
            return '.';
        }
        return image_[y][x];
    }



private:
    Image image_;
};

class Shape {
public:
    explicit Shape(ShapeType type) : type_(type) {}

    void SetPosition(Point pos) {
        position_ = pos;
    }

    void SetSize(Size size) {
        size_ = size;
    }

    void SetTexture(std::shared_ptr<Texture> texture) {
        texture_ = texture;
    }

    void Draw(Image& image) const {
        if (type_ == ShapeType::RECTANGLE) {
            DrawRectangle(image);
        }
        else if (type_ == ShapeType::ELLIPSE) {
            DrawEllipse(image);
        }
    }

private:
    ShapeType type_;
    Point position_;
    Size size_;
    std::shared_ptr<Texture> texture_;

    bool IsPointInsideEllipse(const Point& p) const {
        double a = size_.width / 2.0;
        double b = size_.height / 2.0;
        double x = p.x - position_.x - a;
        double y = p.y - position_.y - b;
        return (x * x) / (a * a) + (y * y) / (b * b) <= 1.0;
    }

    void DrawRectangle(Image& image) const {
        const char pixel = texture_ ? texture_->GetPixel() : '.';
        for (int y = position_.y; y < position_.y + size_.height; ++y) {
            if (y >= image.size()) {
                break;
            }
            std::string& row = image[y];
            for (int x = position_.x; x < position_.x + size_.width; ++x) {
                if (x >= row.size()) {
                    break;
                }
                if (IsPointInsideEllipse({ x, y })) {
                    row[x] = pixel;
                }
            }
        }
    }

    void DrawEllipse(Image& image) const {
        const char pixel = texture_ ? texture_->GetPixel() : '.';
        double a = size_.width / 2.0;
        double b = size_.height / 2.0;
        double x0 = position_.x + a;
        double y0 = position_.y + b;

        for (int y = position_.y; y < position_.y + size_.height; ++y) {
            if (y >= image.size()) {
                break;
            }
            std::string& row = image[y];
            for (int x = position_.x; x < position_.x + size_.width; ++x) {
                if (x >= row.size()) {
                    break;
                }
                if (IsPointInsideEllipse({ x, y })) {
                    double dx = x - x0;
                    double dy = y - y0;
                    if (std::abs(dx / a) < 0.5 && std::abs(dy / b) < 0.5) {
                        row[x] = pixel;
                    }
                }
            }
        }
    }
};
