__draw_hline(0, 100, 120, 0);

__set_pixel(10, 10, 0);

for (var x = 10; x <= 20; x++) print(x);

g.setColor(0,0,0);
print(__make_color(0,0,0));
print(__make_color(1,1,1));

g.reset();
g.setPixel(10, 10);
g.drawLine(0,0, __W-1, __H-1);
g.drawCircle(120, 120, 60); now();
g.fillRect(120, 120, 160, 160); now();
g.clear(); now();
g.drawEllipse(100, 100, 199, 160); now();
g.fillCircle(120, 120, 60); now();
r = [ 120, 0, 200, 120, 10, 70, 20, 30 ]; g.fillPoly(r, 1); now();

g.setFont("", 5); g.drawString("hello world", 10, 10); now();

g.setFont("", 3); g.drawString("hello world", 10, 70); now();

now(); print(new Date()); drawClock();
