/*
 * Interface to C
 */

function __draw_hline(x1, x2, y, color) {
  var temp;

  if (x1 > x2) {
    temp = x1;
    x1 = x2;
    x2 = temp;
  }

  for (var x = x1; x <= x2; x++) {
    __set_pixel(x, y, color);
  }
}

function __fill_rect(x1, y1, x2, y2, color) {
  var temp;

  if (x1 > x2) {
    temp = x1;
    x1 = x2;
    x2 = temp;
  }
  if (y1 > y2) {
    temp = y1;
    y1 = y2;
    y2 = temp;
  }

  for (var y = y1; y <= y2; y++) {
    __draw_hline(x1, x2, y, color);
  }
}

var __redraw = 0;
var __last_t = -1;
function __step(i) {
  if (0) {
    // FIXME
  var t = Date.now();
  if (__last_t != t) {
    __last_t = t;
    drawClock();
  }
  }
  if (__redraw > 0) {
    __redraw = 0;
    return 1;
  }
  return 0;
}

function __touch(d, x, y) {
  drag = {}
  drag.b = d;
  drag.x = x;
  drag.y = y;
  __onTouch(drag);
  return 0;
}

function __key(press, v) {
  /* FIXME */
  print('Key:', press, v);
  return 0;
}

/*
 * Graphics tests / example; they should go to module somewhere
 */

var __ink_color = 0;

function now() { __redraw = 1; }

function sink(x, y) {
  __set_pixel(x, y, __ink_color);
}

function bink(x, y) {
  __set_pixel(x, y, __ink_color);
  __set_pixel(x + 1, y, __ink_color);
  __set_pixel(x, y + 1, __ink_color);
  __set_pixel(x + 1, y + 1, __ink_color);
}

function cls() {
  var color = __ink_color;
  for (var y = 0; y < __screen_height; y++) {
    for (var x = 0; x < __screen_width; x++) {
      __set_pixel(x, y, color);
    }
  }
  __redraw = 1;
  __ink_color = 1 - color;
  now();
}

function glyph() {
  var w = __screen_width, h = __screen_height;
  var steps = Math.max(w, h);
  if (steps > 1) {
    for (var i = 0; i < steps; i++) {
      var x = Math.floor(i * (w - 1) / (steps - 1));
      var y = Math.floor(i * (h - 1) / (steps - 1));
      sink(x, y);
      sink(x, h - 1 - y);
    }
  }

  var cx = (w - 1) / 2, cy = (h - 1) / 2;
  var radius = Math.floor(Math.min(w, h) / 4);
  var thickness = 1.5;
  for (var y = 0; y < h; y++) {
    for (var x = 0; x < w; x++) {
      var dx = x - cx, dy = y - cy;
      var distance = Math.sqrt(dx * dx + dy * dy);
      if (Math.abs(distance - radius) <= thickness) sink(x, y);
    }
  }
  now();
}

function grey() {
  for (var y = 0; y < __screen_height; y++) {
    for (var x = 0; x < __screen_width; x++) {
      if (((x + y) % 2) !== 0) sink(x, y);
    }
  }
  now();
}


/*
 * Graphics implementation
 *
 */

//g.getWidth = function() { return ; };
//g.getHeight = function() { return ; };

var __W = __screen_width;
var __H = __screen_height;
var __BPP = 1;
var __color;
var __bgColor;

function __draw_hline(x1, x2, y, color) {
  var t;
  var x;

  if (x1 > x2) {
    t = x1;
    x1 = x2;
    x2 = t;
  }

  y = Math.round(y);
  x1 = Math.ceil(x1);
  x2 = Math.floor(x2);

  if (y < 0 || y >= __H) return;
  if (x1 < 0) x1 = 0;
  if (x2 >= __W) x2 = __W - 1;

  for (x = x1; x <= x2; x++)
    __set_pixel(x, y, color);
}

function __fill_rect(x1, y1, x2, y2, color) {
  var t;
  var y;

  if (x1 > x2) {
    t = x1; x1 = x2; x2 = t;
  }
  if (y1 > y2) {
    t = y1; y1 = y2; y2 = t;
  }

  for (y = Math.ceil(y1); y <= Math.floor(y2); y++)
    __draw_hline(x1, x2, y, color);
}

function __draw_line(x1, y1, x2, y2, color) {
  var dx = Math.abs(x2 - x1);
  var sx = x1 < x2 ? 1 : -1;
  var dy = -Math.abs(y2 - y1);
  var sy = y1 < y2 ? 1 : -1;
  var err = dx + dy;
  var e2;

  x1 = Math.round(x1);
  y1 = Math.round(y1);
  x2 = Math.round(x2);
  y2 = Math.round(y2);

  while (true) {
    __set_pixel(x1, y1, color);
    if (x1 === x2 && y1 === y2) break;

    e2 = 2 * err;
    if (e2 >= dy) { err += dy; x1 += sx; }
    if (e2 <= dx) { err += dx; y1 += sy; }
  }
}

function __make_color(r, green, b) {
  if (green >= 0.5)
    return 1;
  return 0;
}

function __ellipse(x1, y1, x2, y2, filled, color) {
  var left = Math.min(x1, x2);
  var right = Math.max(x1, x2);
  var top = Math.min(y1, y2);
  var bottom = Math.max(y1, y2);
  var cx = (left + right) / 2;
  var cy = (top + bottom) / 2;
  var rx = (right - left) / 2;
  var ry = (bottom - top) / 2;
  var y;
  var q;
  var half;
  var x;

  if (rx === 0 || ry === 0) {
    if (filled) __fill_rect(left, top, right, bottom, color);
    else {
      __draw_line(left, top, right, bottom, color);
      __draw_line(left, bottom, right, top, color);
    }
    return;
  }

  for (y = Math.ceil(top); y <= Math.floor(bottom); y++) {
    q = 1 - ((y - cy) * (y - cy)) / (ry * ry);
    if (q < 0) continue;

    half = rx * Math.sqrt(q);
    if (filled) {
      __draw_hline(Math.ceil(cx - half), Math.floor(cx + half), y, color);
    } else {
      __set_pixel(Math.round(cx - half), y, color);
      __set_pixel(Math.round(cx + half), y, color);
    }
  }

  if (!filled) {
    for (x = Math.ceil(left); x <= Math.floor(right); x++) {
      q = 1 - ((x - cx) * (x - cx)) / (rx * rx);
      if (q >= 0) {
        half = ry * Math.sqrt(q);
        __set_pixel(x, Math.round(cy - half), color);
        __set_pixel(x, Math.round(cy + half), color);
      }
    }
  }
}

function __polygon(poly, closed, filled, color) {
  var n = Math.floor(poly.length / 2);
  var i;
  var j;
  var x1;
  var y1;
  var x2;
  var y2;
  var ymin;
  var ymax;
  var y;
  var xs;
  var k;
  var tmp;

  if (n < 2) return;

  if (!filled) {
    for (i = 0; i < n - 1; i++) {
      __draw_line(poly[i * 2], poly[i * 2 + 1],
                  poly[i * 2 + 2], poly[i * 2 + 3], color);
    }
    if (closed) {
      __draw_line(poly[(n - 1) * 2], poly[(n - 1) * 2 + 1],
                  poly[0], poly[1], color);
    }
    return;
  }

  ymin = ymax = poly[1];
  for (i = 1; i < n; i++) {
    y = poly[i * 2 + 1];
    if (y < ymin) ymin = y;
    if (y > ymax) ymax = y;
  }

  for (y = Math.ceil(ymin); y <= Math.floor(ymax); y++) {
    xs = [];

    for (i = 0; i < n; i++) {
      j = (i + 1) % n;
      x1 = poly[i * 2];
      y1 = poly[i * 2 + 1];
      x2 = poly[j * 2];
      y2 = poly[j * 2 + 1];

      if ((y1 <= y && y2 > y) || (y2 <= y && y1 > y))
        xs.push(x1 + (y - y1) * (x2 - x1) / (y2 - y1));
    }

    xs.sort(function (a, b) { return a - b; });

    for (k = 0; k + 1 < xs.length; k += 2)
      __draw_hline(Math.ceil(xs[k]), Math.floor(xs[k + 1]), y, color);
  }
}

var g = {
  setPixel: function (x, y) {
    __set_pixel(x, y, __color);
    return g;
  },

  clear: function (reset) {
    if (reset) g.reset();
    __fill_rect(0, 0, __W, __H, __bgColor);
    return g;
  },

  clearRect: function (x1, y1, x2, y2) {
    __fill_rect(x1, y1, x2, y2, __bgColor);
    return g;
  },

  drawCircle: function (x, y, rad) { // FIXME: broken
    var points = [];
    var steps = Math.max(12, Math.ceil(2 * Math.PI * Math.abs(rad) * 2));
    var i;
    var a;

    for (i = 0; i < steps; i++) {
      a = 2 * Math.PI * i / steps;
      points.push(x + Math.cos(a) * rad);
      points.push(y + Math.sin(a) * rad);
    }

    __polygon(points, true, false, __color);
    return g;
  },

  drawEllipse: function (x1, y1, x2, y2) {
    __ellipse(x1, y1, x2, y2, false, __color);
    return g;
  },

  drawLine: function (x1, y1, x2, y2) {
    __draw_line(x1, y1, x2, y2, __color);
    return g;
  },

  drawPoly: function (poly, closed) {
    __polygon(poly, !!closed, false, __color);
    return g;
  },

  drawRect: function (x1, y1, x2, y2) {
    __draw_hline(x1, x2, y1, __color);
    __draw_hline(x1, x2, y2, __color);
    __draw_line(x1, y1, x1, y2, __color);
    __draw_line(x2, y1, x2, y2, __color);
    return g;
  },

  fillCircle: function (x, y, rad) {
    var yy;
    var half;
    rad = Math.abs(rad);

    for (yy = Math.ceil(y - rad); yy <= Math.floor(y + rad); yy++) {
      half = rad * Math.sqrt(Math.max(
        0, 1 - ((yy - y) * (yy - y)) / (rad * rad || 1)
      ));
      __draw_hline(Math.ceil(x - half), Math.floor(x + half), yy, __color);
    }
    return g;
  },

  fillEllipse: function (x1, y1, x2, y2) {
    __ellipse(x1, y1, x2, y2, true, __color);
    return g;
  },

  fillPoly: function (poly) {
    __polygon(poly, true, true, __color);
    return g;
  },

  fillRect: function (x1, y1, x2, y2) {
    __fill_rect(x1, y1, x2, y2, __color);
    return g;
  },

  flip: function (all) {
    return g;
  },

  getBgColor: function () { return __bgColor; },
  getBPP: function () { return __BPP; },
  getColor: function () { return __color; },
  getHeight: function () { return __H; },

  getPixel: function (x, y) {
    return __get_pixel(x, y);
  },

  getWidth: function () { return __W; },

  reset: function () {
    __color = 0;
    __bgColor = 1;
    return g;
  },

  setBgColor: function (r, green, b) {
    __bgColor = __make_color(r, green, b);
    return g;
  },

  setColor: function (r, green, b) {
    __color = __make_color(r, green, b);
    return g;
  },

  setPixel: function (x, y, col) {
    __set_pixel(x, y, col === undefined ? __color : col);
    return g;
  }
};

var __font3x5 = {
  "A":"2,5,7,5,5", "B":"6,5,6,5,6", "C":"3,4,4,4,3",
  "D":"6,5,5,5,6", "E":"7,4,6,4,7", "F":"7,4,6,4,4",
  "G":"3,4,5,5,3", "H":"5,5,7,5,5", "I":"7,2,2,2,7",
  "J":"1,1,1,5,2", "K":"5,5,6,5,5", "L":"4,4,4,4,7",
  "M":"5,7,7,5,5", "N":"5,7,7,7,5", "O":"2,5,5,5,2",
  "P":"6,5,6,4,4", "Q":"2,5,5,3,1", "R":"6,5,6,5,5",
  "S":"3,4,2,1,6", "T":"7,2,2,2,2", "U":"5,5,5,5,7",
  "V":"5,5,5,5,2", "W":"5,5,7,7,5", "X":"5,5,2,5,5",
  "Y":"5,5,2,2,2", "Z":"7,1,2,4,7",

  "0":"7,5,5,5,7", "1":"2,6,2,2,7", "2":"6,1,7,4,7",
  "3":"6,1,3,1,6", "4":"5,5,7,1,1", "5":"7,4,6,1,6",
  "6":"3,4,7,5,7", "7":"7,1,2,2,2", "8":"7,5,7,5,7",
  "9":"7,5,7,1,6",

  "-":"0,0,7,0,0", "_":"0,0,0,0,7", ".":"0,0,0,0,2",
  ":":"0,2,0,2,0", "!":"2,2,2,0,2", "?":"6,1,2,0,2",
  "/":"1,1,2,4,4", "+":"0,2,7,2,0", " ":"0,0,0,0,0"
};

var __fontScale = 1;
var __fontAlignX = -1;
var __fontAlignY = -1;

function __glyphWidth() {
  return 4 * __fontScale; // 3 pixels plus one-pixel spacing, scaled
}

function __draw_glyph(ch, x, y) {
  var rows = __font3x5[ch] || __font3x5[" "];
  var row;
  var col;
  var bit;
  var sx;
  var sy;

  rows = rows.split(",");

  for (row = 0; row < 5; row++) {
    for (col = 0; col < 3; col++) {
      bit = 1 << (2 - col);
      if ((parseInt(rows[row], 10) & bit) !== 0) {
        for (sy = 0; sy < __fontScale; sy++) {
          for (sx = 0; sx < __fontScale; sx++) {
            __set_pixel(
              x + col * __fontScale + sx,
              y + row * __fontScale + sy,
              __color
            );
          }
        }
      }
    }
  }
}

g.setFont = function (name, size) {
  size = size / 6;
  __fontScale = Math.max(1, Math.floor(size || 1));
  return g;
};

g.setFontAlign = function (x, y) {
  __fontAlignX = x || 0;
  __fontAlignY = y || 0;
  return g;
};

g.drawString = function (text, x, y) {
  var s = String(text);
  var width = s.length ? (s.length * 4 - 1) * __fontScale : 0;
  var height = 5 * __fontScale;
  var i;
  var ch;

  if (__fontAlignX === 0) x -= width / 2;
  else if (__fontAlignX > 0) x -= width;

  if (__fontAlignY === 0) y -= height / 2;
  else if (__fontAlignY > 0) y -= height;

  x = Math.round(x);
  y = Math.round(y);

  for (i = 0; i < s.length; i++) {
    ch = s.charAt(i).toUpperCase();
    __draw_glyph(ch, x + i * 4 * __fontScale, y);
  }

  return g;
};

g.wrapString = function (text, maxWidth) {
  var words = String(text).split(/\s+/);
  var lines = [];
  var line = "";
  var word;
  var candidate;
  var maxChars = Math.floor(maxWidth / (4 * __fontScale));

  if (maxChars < 1) maxChars = 1;

  for (var i = 0; i < words.length; i++) {
    word = words[i];
    candidate = line ? line + " " + word : word;

    if (candidate.length > maxChars && line) {
      lines.push(line);
      line = word;
    } else {
      line = candidate;
    }
  }

  if (line) lines.push(line);
  return lines;
};

/*
 * Ink/special support for T-Dev-Max
 */

var ink = {
  isInk: function () { return true; },
  update: function () { __redraw = 1; }
};

/*
 * Input and dummy functions
 */

var bangle_on_map = {}

function bangle_on(event, callback) {
  bangle_on_map[event] = callback;
  if (event == "lock")
    callback();
}

function bangle_setUI(map) {
  if (map.drag) {
    bangle_on_map['drag'] = map.drag;
  }
}

function __onTouch(drag) {
  var d = bangle_on_map['drag'];
  if (d) {
    d(drag);
  }
  var d = bangle_on_map['touch'];
  if (d) {
    d(drag.b, drag);
  }
}

// Some functionality is available in pavel's Espruino tree
Bangle = {};
Bangle.setGPSPower = function () { print("setGPSpower() not implemented"); };
Bangle.loadWidgets = function () { };
Bangle.drawWidgets = function () { };
Bangle.setUI =  bangle_setUI;
Bangle.project = function () { print("project unimplemented, available elsewhere") };
Bangle.isCharging = function () { return false; }
Bangle.isCompassOn = function () { return false; }
Bangle.setCompassPower = function (v) {}
Bangle.getAccel = function (v) { print("getAccel() not implemented"); }
Bangle.on = bangle_on;
Bangle.buzz = function (v) { print("Buzz requested"); }
Bangle.getGPSFix = function () { print("gps not implemented"); }
Bangle.getCompass = function () { print("compass not implemented"); }
Bangle.isLocked = function() { return false; }
Bangle.getHealthStatus = function() { return { steps : 1234 } }
Bangle.http = function () { print("http not implemented, available elsewhere"); }
Puck = {}
Puck.light = function () { print("light not implemented"); }
WIDGETS = false;
E = {};
E.getBattery = function () { return 100; }
const BTN1 = 1;

g.reset();

/*
 * Example application: time display
 */

/* Note: due to js being... bad languagee, you can't just reenable this using if (1) */
if (0) {
  var cols = 3;
  var rows = 10;
  var lastSlot = -1;

  function drawClock() {
    var now = new Date();
    var slot = now.getMinutes() % (cols*rows);
    // 29->30 transition is wrong, it goes from all white to all black
    var ink = Math.floor(now.getMinutes() / (cols*rows)) % 2;
    var w = g.getWidth();
    var h = g.getHeight();
    var cellW = w / cols;
    var cellH = h / rows;
    var i;
    var col;
    var row;
    var x1;
    var y1;
    var x2;
    var y2;
    var time;

    // Redraw only when the minute changes.
    if (slot === lastSlot) return;
    lastSlot = slot;

    g.setBgColor(1-ink, 1-ink, 1-ink);
    g.clear();

    for (i = 0; i < 30; i++) {
      col = i % cols;
      row = Math.floor(i / cols);

      x1 = Math.floor(col * cellW);
      y1 = Math.floor(row * cellH);
      x2 = Math.floor((col + 1) * cellW) - 1;
      y2 = Math.floor((row + 1) * cellH) - 1;

      if (i < slot) {
        g.setColor(ink, ink, ink);
        g.fillRect(x1, y1, x2, y2);
      }

      if (0) {
        g.setColor(1, 1, 1);
        g.drawRect(x1, y1, x2, y2);
      }

      if (i === slot) {
        time = ("0" + now.getHours()).slice(-2) + ":" +
          ("0" + now.getMinutes()).slice(-2);

        print("Time", time);

        g.setColor(ink, ink, ink);
        g.setFont("bitmap", 3);
        g.setFontAlign(0, 0);
        g.drawString(time, (x1 + x2) / 2, (y1 + y2) / 2);
      }
    }

    g.flip();
    __redraw = 1;
  }

//  drawClock();
//  setInterval(drawClock, 1000);
}


/*
 * Example application: drawing
 
Bangle.js2, give me simple, black & white drawing application. Should
allow brush selection and white/black color select.

*/

  var W = g.getWidth();
  var H = g.getHeight();
  var WIDGETS = 0;
  var TOOLBAR = WIDGETS+26;
  var color = 0;
  var brush = 2;
  var brushes = [1, 2, 4];
  var last = null;

  function black() {
    g.setColor(0, 0, 0);
  }

  function white() {
    g.setColor(1, 1, 1);
  }

  function toolbar() {
    white();
    g.fillRect(0, WIDGETS, W, TOOLBAR);

    black();
    g.drawLine(0, TOOLBAR, W, TOOLBAR);

    g.setFont("Vector", (TOOLBAR - WIDGETS) * 0.5);

    var y1 = WIDGETS + 4;
    var y2 = y1 + 4;
    var y3 = WIDGETS + TOOLBAR - 4;

    // Black brush
    g.fillRect(5, y1, 40, y3);
    g.setColor(1, 1, 1);
    g.drawString("B", 19, y2);

    // White brush
    g.setColor(1, 1, 1);
    g.fillRect(48, y1, 83, y3);
    black();
    g.drawRect(48, y1, 83, y3);
    g.drawString("W", 58, y2);

    // Brush size
    black();
    g.drawString("S" + brush, 91, y2);

    // Clear
    g.drawRect(137, y1, 171, y3);
    g.drawString("CLR", 140, y2);
  }

  function clearCanvas() {
    white();
    g.fillRect(0, TOOLBAR+1, W - 1, H - 1);
  }

  function drawPoint(x, y, px, py) {
    g.setColor(color, color, color);
    //g.setLineWidth(brushes[brush - 1]);

    if (px !== undefined) {
      g.drawLine(px, py, x, y);
    } else {
      g.fillCircle(x, y, brushes[brush - 1]);
    }
  }

  function redraw() {
    toolbar();
  }

  function touch(e) {
    if (e.y < WIDGETS || e.y > TOOLBAR) return;

    if (e.x < 46) {
      print("Draw");
      color = 0;
    } else if (e.x < 87) {
      print("Erase");
      color = 1;
    } else if (e.x < 134) {
      brush++;
      if (brush > 3) brush = 1;
      print("Brush", brush);
    } else {
      print("Clear");
      clearCanvas();
    }

    last = null;
    redraw();
  }

  function drag(e) {
    if (e.b) {
      if (e.y > TOOLBAR) {
        var y = e.y;
        var x = e.x;

        drawPoint(x, y,
                  last ? last.x : undefined,
                  last ? last.y : undefined);

        last = {x:x, y:y};
      } else touch(e);
    } else {
      ink.update();
      last = null;
    }
  }

  // Start application
  g.clear(1);
  clearCanvas();
  Bangle.loadWidgets();
  Bangle.drawWidgets();
  toolbar();

  Bangle.on('drag', drag);

  if (0)
  Bangle.setUI({
    mode: "custom",
    back: function() {
      load();
    },
    drag: drag
  });


// FIXME: There really should be ^@ at end of file, but that interferes with git
 
