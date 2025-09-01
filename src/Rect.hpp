class Rect
{
public:
  int x, y, w, h;
  Rect()
  {
  }
  Rect(int x_, int y_, int w_, int h_) : x(x_), y(y_), w(w_), h(h_)
  {
  }
  Rect padding(int p)
  {
    return {x + p, y + p, w - p * 2, h - p * 2};
  }
  Rect grid(int gw, int gh, int gx, int gy, int gsw = 1, int gsh = 1)
  {
    return {
        x + w / gw * gx,
        y + h / gh * gy,
        w / gw * gsw,
        h / gh * gsh};
  }
  bool isIn(int px, int py)
  {
    return px >= x && py >= y && px <= x + w && py <= y + h;
  }
};