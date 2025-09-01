class SwitchButton
{
public:
  Rect r={10,10,100,50};
  bool state=false;
  String text="Kitchen";
  String entity="";
  String onstate = "ON";
  String offstate = "OFF";
  SwitchButton(){}

  SwitchButton(int x, int y, int w, int h, String text_, String entity_)
  {
    r = Rect(x, y, w, h);
    text = text_;
    entity = entity_;
  }
  void draw()
  {
    auto cp = r.padding(p);
    uint32_t col = state ? TFT_ORANGE : TFT_DARKGREY;
    tft.drawSmoothRoundRect(r.x, r.y, p, 0, r.w, r.h, col, TFT_BLACK);
    tft.fillRect(cp.x, cp.y, cp.w, cp.h, col);
    tft.fillRect(cp.x, cp.y, 10, 10, TFT_WHITE);
    tft.setTextColor(TFT_WHITE);
    
    tft.setTextFont(4);
    tft.setTextSize(1);
    tft.setTextDatum(TC_DATUM);

    tft.drawString(text, cp.x+cp.w/2, cp.y + cp.h - 15 - p);
  }
};