#ifndef GAME_H
#define GAME_H

#include "gdi.hpp"

namespace game {




  int POC_FLAG = 0;
  int MOV_NEXT = 0;
  int ROT_NEXT = 1;



  int ROT = 1; // (st)
  int MOV = 2; // (st)


  // conctants

  int subcolumns_num = 4;
  int subrows_num = 6;
  int points_num = 2; // Math.ceil(subrows_num / 2) - 1; // bottom mirrors top

  int wx = 400;            // window width (can be changed in runtime without harm)
  int wy = 300;            // window height
  int cn = 7;              // columns number (CANNOT BE LESS THAN 3 AND MORE THAN 8 * sizeof(wx))
  int details_reduction = 0; // skips column details from further (zero or more)

  int columns_w_subc_num = cn - 1; // last column subcolumns doesn't exists

  float* cx; // column x positions

  int cx_init() {

    cx = new float [cn];
    cx[0] = -wx / 2;
    cx[1] = 0;
    cx[cn-1] = wx / 2;
    for (int i = 2; i < cn - 1; ++i) {
      cx[i] = cx[i-1] + (wx >> i);
    }

  }

  float rs = wx / 50.0;    // rotation speed (pixel per frame)
  float ms = 50;           // move speed (frame per second) (DO NOT SET LESS THAN 50 or change 'move exit' logic)
  float ad = (float)wx / wy / ms; // acceleration delta
  float ac = 1;              // current acceleration
  int st = 0;              // drawing status
  int mx = 1;              // mirror (+1:no, -1:yes)
  float dx = 0;              // offset (changes on rotation)


  // initialization

  struct POINT {
    float y;
    float sy;
  };

  struct SUBC {
    POINT* points;
    float x;
    float y;
    float sx;
    float sy;
  };

  struct COL {
    SUBC* subcolumns;
    float x;
    float y;
    float dx;
    float dy;
    float sx;
    float sy;
  };

  struct WALL {
    int **ma;
    float pa;
    COL* cl;
    COL* cr;
    float lx;      // position left
    float ly;
    float rx;     // position right
    float ry;
    float ox; // position center
    float oy;
    float px; // pointer (middot)
    float py;
  };



  WALL w1;
  WALL w2;


  HDC hdc;




  const int MAP_SIZE = 8;
  /*
  const int map[MAP_SIZE][MAP_SIZE] = {
    {1,1,1,1,1,1,1,1},
    {1,1,0,0,0,1,0,1},
    {1,0,0,1,0,0,0,1},
    {1,1,0,1,1,1,0,1},
    {1,0,0,0,1,0,0,1},
    {1,0,1,0,1,0,1,1},
    {1,0,1,0,0,0,0,1},
    {1,1,1,1,1,1,1,1}
  };
  */
  const int map[MAP_SIZE][MAP_SIZE] = {
    {1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,0,1},
    {1,0,1,1,1,1,0,1},
    {1,0,1,0,1,1,0,1},
    {1,0,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1}
  };
  int player_x = 1, player_y = 4; // player position
  int px = player_x, py = player_y; // camera position
  int dir = 0; // 0: top, 1: right, 2: down, 3: left
  int rot = 1; // left(forward): -1, right: +1
  // enums
  int F_PRC = 0; // (flg) camera in cell (waits for new command)
  int F_MOV = 1; // (flg) camera moving
  int F_ROT = 2; // (flg) camera rotating
  int F_AHD = 3; // (flg) camera in deadend (must rotate 2 times)
  int flg = POC_FLAG;
  int FPS = 60; // frames per second
  float MSPF = 1000.f / FPS; // milliseconds per frame
  float prev_timestamp = 0;

  int minimap() {
    gdi::style(RGB(128,128,128));
    for (int x = 0; x < MAP_SIZE; ++x)
      for (int y = 0; y < MAP_SIZE; ++y)
    {
      if (map[x][y]) {
        gdi::rect(hdc, 10 + x * 11, 100 + y * 11, 10, 10);
      }
    }
    gdi::style(RGB(0,128,128));
    gdi::rect(hdc, 10 + px * 11, 100 + py * 11, 10, 10);
  }

  int dumpmap(WALL& w, int offs) {
    for (int x = 0; x < 3; ++x)
      for (int y = 0; y < cn; ++y)
    {
      if (w.ma[x][y] == 1) {
        gdi::style(RGB(128,128,128));
        gdi::rect(hdc, offs + x * 11, 250 - y * 11, 10, 10);
      }
      if (w.ma[x][y] == 2) {
        gdi::style(RGB(128,0,0));
        gdi::rect(hdc, offs + x * 11, 250 - y * 11, 10, 10);
      }
    }
  }



  // for draw_wall
  float doors_buffer [8];
  float columns_buffer [8];
  float olds_buffer [8];



  WALL define_wall(float middot_speed) {
    WALL w;
    w.ma = new int* [3];
    for (int i = 0; i < 3; ++i) {
      w.ma[i] = new int [cn];
      for (int k = 0; k < cn; ++k) {
        w.ma[i][k] = 0;
      }
    }
    w.pa = middot_speed;
    w.cl = new COL [cn];
    for (int i = 0; i < cn; ++i) {
      w.cl[i].subcolumns = new SUBC [subcolumns_num];
      for (int k = 0; k < subcolumns_num; ++k) {
        w.cl[i].subcolumns[k].points = new POINT [points_num];
      }
    }
    w.cr = new COL [cn];
    for (int i = 0; i < cn; ++i) {
      w.cr[i].subcolumns = new SUBC [subcolumns_num];
      for (int k = 0; k < subcolumns_num; ++k) {
        w.cr[i].subcolumns[k].points = new POINT [points_num];
      }
    }
    return w;
  }


  int reset_main_wall() {
    w1.lx = 0;      // position left
    w1.ly = 0;
    w1.rx = wx;     // position right
    w1.ry = 0;
    w1.ox = wx / 2; // position center
    w1.oy = wy / 2;
    w1.px = wx / 2; // pointer (middot)
    w1.py = wy / 2;

    for (int i = 0; i < cn; ++i) {
      // ===== column by left ========
      w1.cl[i].x  = cx[i];
      w1.cl[i].y  = cx[i] * wy / wx;
      w1.cl[i].dx = i > 0 ? w1.cl[i - 1].x : 0;
      w1.cl[i].dy = w1.cl[i].y;
      w1.cl[i].sx = i > 0 ? ((w1.cl[i - 1].x - w1.cl[i].x) / ms) : 0;
      w1.cl[i].sy = w1.cl[i].sx * wy / wx;

      // ===== column by right ======
      w1.cr[i].x  = -w1.cl[i].x; // column positions
      w1.cr[i].y  =  w1.cl[i].y;
      w1.cr[i].dx = -w1.cl[i].dx; // door positions
      w1.cr[i].dy =  w1.cr[i].y;

      // ====== subcolumns ======

      if (i < columns_w_subc_num) {

        float column_w = cx[i + 1] - cx[i];
        float subcolumn_w = column_w / (subcolumns_num + 0);

        float acc_x = cx[i]; // default previous x position (accumulator)


        for (int k = 0; k < subcolumns_num; ++k) {
          acc_x += subcolumn_w;
          // left subcolumns
          w1.cl[i].subcolumns[k].x  = acc_x;
          w1.cl[i].subcolumns[k].y  = w1.cl[i].subcolumns[k].x * wy / wx; // y position
          w1.cl[i].subcolumns[k].sx = i > 0 ? ((w1.cl[i - 1].subcolumns[k].x - w1.cl[i].subcolumns[k].x) / ms) : 0;
          w1.cl[i].subcolumns[k].sy = w1.cl[i].subcolumns[k].sx * wy / wx;
          // right subcolumns
          w1.cr[i].subcolumns[k].x = -w1.cl[i].subcolumns[k].x;
          w1.cr[i].subcolumns[k].y = +w1.cl[i].subcolumns[k].y;



          float sLPh = (wy - w1.cl[i].subcolumns[k].y * 2) / subrows_num; // left subcolumn points height (sLPh same as sRPh)

          for (int t = 0; t < points_num; ++t) {
            //const pL = w1.cl[i].subcolumns[k].points[t];
            w1.cl[i].subcolumns[k].points[t].y  = w1.cl[i].subcolumns[k].y + sLPh * (t + 1);
            w1.cl[i].subcolumns[k].points[t].sy = i > 0 ? ((w1.cl[i - 1].subcolumns[k].points[t].y - w1.cl[i].subcolumns[k].points[t].y) / ms) : 0;

            w1.cr[i].subcolumns[k].points[t].y  = +w1.cl[i].subcolumns[k].points[t].y;
            w1.cr[i].subcolumns[k].points[t].sy = +w1.cl[i].subcolumns[k].points[t].sy; // no need to define?
          }
        }
      }

    }
  }


  int reset_side_wall() {
    w2.lx = -wx;
    w2.ly =  0;
    w2.rx =  0;
    w2.ry =  0;
    w2.ox = -1;
    w2.oy =  wy / 2;
    w2.px = -wx / 2;
    w2.py =  0;

    for (int i = 0; i < cn; ++i) {
      w2.cl[i].x  = w2.lx;
      w2.cl[i].y  = w1.cl[i].y;
      w2.cl[i].dx = 0;
      w2.cl[i].dy = 0;

      w2.cr[i].x  = -1;
      w2.cr[i].y  = w1.cl[i].y;
      w2.cr[i].dx = 0;
      w2.cr[i].dy = 0;

      // ====== subcolumns ======

      if (i < columns_w_subc_num) {

        float column_w = cx[i+1] - cx[i];
        float subcolumn_w = column_w / (subcolumns_num + 1);

        float acc_x = cx[i];
        for (int k = 0; k < subcolumns_num; ++k) {
          acc_x += subcolumn_w;

          //const sL = w2.cl[i].subcolumns[k];
          w2.cl[i].subcolumns[k].x  = acc_x;
          w2.cl[i].subcolumns[k].y  = w2.cl[i].subcolumns[k].x * wy / wx; // y position


          //const sR = w2.cr[i].subcolumns[k];
          w2.cr[i].subcolumns[k].x = -w2.cl[i].subcolumns[k].x;
          w2.cr[i].subcolumns[k].y = +w2.cl[i].subcolumns[k].y;

          float sLPh = (wy - w2.cl[i].subcolumns[k].y * 2) / subrows_num;
          for (int t = 0; t < points_num; ++t) {
            //const pL = w2.cl[i].subcolumns[k].points[t];
            w2.cl[i].subcolumns[k].points[t].y  = w2.cl[i].subcolumns[k].y + sLPh * (t + 1);
            w2.cr[i].subcolumns[k].points[t].y  = +w2.cl[i].subcolumns[k].points[t].y;
          }



        }
      }
    }
  }


  int w_init() {
    w1 = define_wall(-((float)wy / wx / 2.0) * rs);
    w2 = define_wall(-w1.pa);
    reset_main_wall();
    reset_side_wall();
  }



  int drw(float x1, float y1, float x2, float y2, int depth) {
    gdi::style(RGB(255, 255, 255));
    gdi::stroke(hdc, (int)x1, (int)y1, (int)x2, (int)y2);
  }


  int draw_line(float x1, float y1, float x2, float y2, COLORREF rgb) {
    HPEN pen = CreatePen(PS_SOLID, 1, rgb);
    gdi::stroke(hdc, (int)x1, (int)y1, (int)x2, (int)y2, pen);
    DeleteObject(pen);
  }


  // drw: function (x1, y1, x2, y2, depth)
  int draw_wall(WALL &w) {

    float* d = doors_buffer;
    float* c = columns_buffer;
    float* o = olds_buffer;

    int i = 0;
    for ( ; i < cn; ++i) {

      // left column [x,y,x,y]
      c[0] = (w.cl[i].x + w.lx) * mx + dx;
      c[1] =  w.cl[i].y;
      c[2] =  c[0];
      c[3] =  wy - c[1];

      // right column [x,y,x,y]
      c[4] = (w.cr[i].x + w.rx) * mx + dx;
      c[5] =  w.cr[i].y;
      c[6] =  c[4];
      c[7] =  wy - c[5];

      // left door [x,y,x,y]
      d[0] = (w.cl[i].dx + w.lx) * mx + dx;
      d[1] =  w.cl[i].dy;
      d[2] =  d[0];
      d[3] =  wy - d[1];

      // right door [x,y,x,y]
      d[4] = (w.cr[i].dx + w.rx) * mx + dx;
      d[5] =  w.cr[i].dy;
      d[6] =  d[4];
      d[7] =  wy - d[5];

      if (0 < i) {
        // vertical lines (need only when rotating, but who cares?) (???)
        //drw(c[0], c[1], c[2], c[3], i);
        //drw(c[4], c[5], c[6], c[7], i);
        draw_line(c[0], c[1], c[2], c[3], RGB(255, 255, 250));
        draw_line(c[4], c[5], c[6], c[7], RGB(255, 255, 250));

        draw_line(c[0], c[1], c[0], c[1]-20, RGB(255, 0, 0));


        // draws columns on sides
        if (1 < i) {

          // left wall
          if (w.ma[0][i - 1]) {
            // bricks
            if (i < columns_w_subc_num - details_reduction) {

              for (int k = 0; k < subcolumns_num; ++k) {
                // left subcolumn
                float sLLx = (w.cl[i].subcolumns[k].x + w.lx) * mx + dx;
                float sLLy =  w.cl[i].subcolumns[k].y;
                float sLRx =  sLLx;
                float sLRy =  wy - sLLy;
                draw_line(sLLx, sLLy, sLRx, sLRy, RGB(0, 255, 0));
                // points
                for (int m = 0; m < points_num; ++m) {
                  float pLy = w.cl[i].subcolumns[k].points[m].y;
                  gdi::brush(RGB(0, 0, 0));
                  gdi::pen(RGB(255, 255, 0));
                  gdi::rect(hdc, sLLx, pLy, 5, 5);
                }
              }

            }
            drw(c[0], c[1], o[0], o[1], i);
            drw(c[2], c[3], o[2], o[3], i);
          }
          // left door
          else {
            drw(d[0], d[1], c[0], c[1], i);
            drw(d[2], d[3], c[2], c[3], i);
          }

          // right wall
          if (w.ma[2][i - 1]) {
            drw(c[4], c[5], o[4], o[5], i);
            drw(c[6], c[7], o[6], o[7], i);
          }
          // right door
          else {
            drw(d[4], d[5], c[4], c[5], i);
            drw(d[6], d[7], c[6], c[7], i);
          }
        }

        // wall in front
        if (w.ma[1][i]) {
          drw(c[0], c[1], c[2], c[3], i); // |
          drw(c[4], c[5], c[6], c[7], i);
          drw(c[0], c[1], c[4], c[5], i); // _
          drw(c[2], c[3], c[6], c[7], i);

          break; // no need to draw other ones
        }

      }

      /*
      if (0 < i && i < columns_w_subc_num - details_reduction) {
        // subcolumns
        for (int k = 0; k < subcolumns_num; ++k) {
          // left subcolumn
          float sLLx = (w.cl[i].subcolumns[k].x + w.lx) * mx + dx;
          float sLLy =  w.cl[i].subcolumns[k].y;
          float sLRx =  sLLx;
          float sLRy =  wy - sLLy;

          draw_line(sLLx, sLLy, sLRx, sLRy, RGB(0, 255, 0));

          // right subcolumn
          float sRLx = (w.cr[i].subcolumns[k].x + w.rx) * mx + dx;
          float sRLy =  w.cr[i].subcolumns[k].y;
          float sRRx =  sRLx;
          float sRRy =  wy - sRLy;

          draw_line(sRLx, sRLy, sRRx, sRRy, RGB(0, 0, 255));

          // points

          for (int m = 0; m < points_num; ++m) {
            POINT& pL = w.cl[i].subcolumns[k].points[m];
            //ctx.fillStyle = 'white';
            //ctx.fillRect(sLLx, pL.y, 5, 5);
            gdi::brush(RGB(255, 255, 0));
            gdi::pen(RGB(255, 255, 0));
            gdi::rect(hdc, sLLx, pL.y, 5, 5);

            POINT& pR = w.cr[i].subcolumns[k].points[m];
            //ctx.fillStyle = 'yellow';
            //ctx.fillRect(sRRx, pR.y, 5, 5);
            gdi::brush(RGB(255, 0, 255));
            gdi::pen(RGB(255, 0, 255));
            gdi::rect(hdc, sRRx, pR.y, 5, 5);
          }
        }
      }
      */



      // swap buffers
      //[o, c] = [c, o];
      float* ob = o;
      o = c;
      c = ob;
    }

    // "X" in the end of room (only while moving)
    if (i == cn && st == MOV) {
      drw(o[0], o[1], w.ox * mx + dx, w.oy, cn);
      drw(o[2], o[3], w.ox * mx + dx, w.oy, cn);
      drw(o[4], o[5], w.ox * mx + dx, w.oy, cn);
      drw(o[6], o[7], w.ox * mx + dx, w.oy, cn);
    }

    //ctx.fillStyle = 'red';
    //ctx.fillRect(w1.ox, w1.oy, 2, 2);
    gdi::style(RGB(255, 0, 0));
    gdi::rect(hdc, w1.ox, w1.oy, 2, 2);


    //ctx.fillStyle = 'blue';
    //ctx.fillRect(w1.rx, w1.ry, 10, 10);
    //ctx.fillRect(w1.rx, wy - w1.ry, 10, 10);
    gdi::style(RGB(0, 0, 255));
    gdi::rect(hdc, w1.rx, w1.ry, 10, 10);
    gdi::rect(hdc, w1.rx, wy - w1.ry, 10, 10);

    //ctx.restore();

    //ctx.fillStyle = 'rgba(50, 50, 50, 0.5)';
    //ctx.fillRect(0, 0, ctx.canvas.width, transy); // top
    //ctx.fillRect(0, transy + wy, ctx.canvas.width, ctx.canvas.height); // bottom
    //ctx.fillRect(0, 0, transx, ctx.canvas.height); // left
    //ctx.fillRect(transx + wx, 0, ctx.canvas.width, ctx.canvas.height); // right

    //ctx.font = '20px "Arial"';
    //ctx.fillStyle = 'blue';
    //ctx.fillText('fuck you', 30, 30);

  }



  // uses 'reset_main_wall'
  int move() {
    // setup
    if (st != MOV) {
      st = MOV;
      ac = 1;
    }
    // move exit
    else if (w1.cl[2].x + w1.cl[2].sx * ac <= 0) {
      st = 0;
      reset_main_wall();
      return 1; // over
    }

    ac += ad; // moving

    for (int i = 0; i < cn; ++i) {
      w1.cl[i].x +=  w1.cl[i].sx * ac;
      w1.cl[i].y +=  w1.cl[i].sy * ac;
      w1.cl[i].dx =  0; if (i > 0) w1.cl[i].dx = w1.cl[i - 1].x;
      w1.cl[i].dy =  w1.cl[i].y;

      w1.cr[i].x -=  w1.cl[i].sx * ac; // same as w1.cr[i].sx * ac;
      w1.cr[i].y +=  w1.cl[i].sy * ac; // same as w1.cr[i].sy * ac;
      w1.cr[i].dx = -w1.cl[i].dx;
      w1.cr[i].dy =  w1.cr[i].y;

      // ---- subcolumns ------

      // do not draw last column subcolumns, they are not initialized
      if (i < cn - 1 - details_reduction) {
        for (int k = 0; k < subcolumns_num; ++k) {
          w1.cl[i].subcolumns[k].x += w1.cl[i].subcolumns[k].sx * ac;
          w1.cl[i].subcolumns[k].y += w1.cl[i].subcolumns[k].sy * ac;

          w1.cr[i].subcolumns[k].x -= w1.cl[i].subcolumns[k].sx * ac;
          w1.cr[i].subcolumns[k].y += w1.cl[i].subcolumns[k].sy * ac;

          // points
          for (int t = 0; t < points_num; ++t) {
            w1.cl[i].subcolumns[k].points[t].y += w1.cl[i].subcolumns[k].points[t].sy * ac;
            w1.cr[i].subcolumns[k].points[t].y += w1.cr[i].subcolumns[k].points[t].sy * ac;
          }
        }
      }
    }

    return 0; // moving cycle not over
  }

  // TODO: FASTER INTERSECTION EXIST
  struct XY {
    float x;
    float y;
  };

  XY intersection(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
    float d12 = x1 * y2 - y1 * x2;
    float d34 = x3 * y4 - y3 * x4;
    float d14 = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    return (XY) {
      (d12 * (x3 - x4) - d34 * (x1 - x2)) / d14,
      (d12 * (y3 - y4) - d34 * (y1 - y2)) / d14
    };
  }





  // uses 'intersection', 'reset_main_wall', 'reset_side_wall'
  int rotate(int side) {
    // setup
    if (st != ROT) {
      st = ROT;
      if (side == -1) {
        mx = -1;
        dx = wx;
      }
    } else if (w2.lx + rs >= 0) {
      st = 0;
      mx = 1;
      dx = 0;
      reset_main_wall();
      reset_side_wall();
      return 1; // over
    }

    // applies offset

    w1.rx += rs;
    w1.lx += rs;
    w1.px += rs;
    w1.py += w1.pa;

    w2.rx += rs;
    w2.lx += rs;
    w2.px += rs;
    w2.py += w2.pa;

    w1.ox = w1.lx + (w1.oy - w1.ly) * (w1.px - w1.lx) / (w1.py - w1.ly);
    w2.ox = w2.rx + (w2.oy - w2.ry) * (w2.px - w2.rx) / (w2.py - w2.ry);

    w1.ry = w1.rx / (w2.ox - w1.lx) * (w2.oy - w1.ly);
    w2.ly = w2.lx / (w1.ox - w2.rx) * (w1.oy - w2.ry);

    // redefines columns
    XY I; // intersection buffer
    for (int i = 1; i < cn; ++i) {

      // You MUST calculate intersections after w.px > wx because
      // you need calculated values to draw deadend (front) columns.

      // скорее всего здесь тоже можно избавиться от intersection, как это сделано в subcolumns

      // main wall column
      w1.cl[i].x = w1.cl[i].y * (w1.ox - w1.lx) / (w1.oy - w1.ly);
      I = intersection(w1.rx, w1.ry, w1.ox, w1.oy,
                       w1.cl[i].x + w1.lx, w1.cl[i].y, w2.ox, w2.oy);
      w1.cr[i].x = I.x - w1.rx;
      w1.cr[i].y = I.y;

      // side wall column
      w2.cr[i].x = w2.cr[i].y * (w2.ox - w2.rx) / (w2.oy - w2.ry);
      I = intersection(w2.lx, w2.ly, w2.ox, w2.oy,
                       w2.cr[i].x + w2.rx, w2.cr[i].y, w1.ox, w1.oy);
      w2.cl[i].x = I.x - w2.lx;
      w2.cl[i].y = I.y;

      // HACK: to delete bugged door lines from screen in the end of rotation. (???)
      if (w1.cl[i].x < -wx) w1.cl[i].x = 0;
      if (w1.cr[i].x < -wx) w1.cr[i].x = 0;

      // TODO: you dont need calculate subcolumns if there is a door (do check w.ma)

      // do not draw last column subcolumns, they are not initialized
      if (i < cn - 1 - details_reduction) {

        // main wall subcolumns

        float sRw = (w1.cr[i + 1].x - w1.cr[i].x) / (subcolumns_num + 1); // right subcolumn width
        float sR_prev_x = w1.cr[i].x; // right subcolumn previous x position (accumulator)

        for (int k = 0; k < subcolumns_num; ++k) {
          // left subcolumn
          SUBC& sL = w1.cl[i].subcolumns[k];
          sL.x = sL.y * (w1.ox - w1.lx) / (w1.oy - w1.ly);

          // right subcolumn (you can do it with intersection -- slower but lighter)
          SUBC& sR = w1.cr[i].subcolumns[k];
          sR.x = sR_prev_x + sRw;
          float W0 = w1.rx - w1.ox;
          float W1 = W0 + sR.x; if (W1 < 0) W1 = 0;
          sR.y = w1.oy - (w1.oy - w1.ry) * W1 / W0;
          sR_prev_x = sR.x;

          // right subcolumn points

          float sRPh = (wy - sR.y * 2) / subrows_num; // right subcolumn point height
          float sRP_prev_y = sR.y; // accumulator

          for (int t = 0; t < points_num; ++t) {
            sRP_prev_y += sRPh; // increase acc first
            sR.points[t].y = sRP_prev_y; // right points (left column y'es not changing)
          }
        }

        // side wall subcolumns (inverse calculations)

        float sLw = (w2.cl[i + 1].x - w2.cl[i].x) / (subcolumns_num + 1); // left subcolumn width
        float sL_prev_x = w2.cl[i].x; // left subcolumn previous x position (accumulator)

        for (int k = 0; k < subcolumns_num; ++k) {
          // left subcolumn
          SUBC& sL = w2.cl[i].subcolumns[k];
          sL.x = sL_prev_x + sLw;
          float W0 = w2.lx - w2.ox;
          float W1 = W0 + sL.x;
          sL.y = w1.oy - (w2.oy - w2.ly) * W1 / W0;
          sL_prev_x = sL.x;

          // right subcolumn
          SUBC& sR = w2.cr[i].subcolumns[k];
          sR.x = sR.y * (w2.ox - w2.rx) / (w2.oy - w2.ry);

          // left subcolumn points (inverse calculations) (probably could do without inverse)

          float sLPh = (wy - sL.y * 2) / subrows_num;
          float sLP_prev_y = sL.y;

          for (int t = 0; t < points_num; ++t) {
            sLP_prev_y += sLPh;
            sL.points[t].y = sLP_prev_y;
          }
        }
      }

      // doors

      if (i > 1) {
        int u = i - 1;

        // TODO: you dont need calculate dx, dy if there is no doors (do check w.ma)



          // main wall left
          I = intersection(w1.cl[u].x, w1.cl[u].y, w1.cl[u].x, wy - w1.cl[u].y,
                           w1.cl[i].x, w1.cl[i].y, w2.ox - w1.lx, w2.oy - w1.ly);
          w1.cl[i].dx = I.x;
          w1.cl[i].dy = I.y;



          // main wall right
          I = intersection(w1.cr[u].x, w1.cr[u].y, w1.cr[u].x, wy - w1.cr[u].y,
                           w1.cr[i].x, w1.cr[i].y, w2.ox - w1.lx, w2.oy - w1.ly);
          w1.cr[i].dx = I.x;
          w1.cr[i].dy = I.y;



          // side wall left
          I = intersection(w2.cl[u].x, w2.cl[u].y, w2.cl[u].x, wy - w2.cl[u].y,
                           w2.cl[i].x, w2.cl[i].y, w1.ox - w2.rx, w1.oy - w2.ry);
          w2.cl[i].dx = I.x;
          w2.cl[i].dy = I.y;



          // side wall right
          I = intersection(w2.cr[u].x, w2.cr[u].y, w2.cr[u].x, wy - w2.cr[u].y,
                           w2.cr[i].x, w2.cr[i].y, w1.ox - w2.rx, w1.oy - w2.ry);
          w2.cr[i].dx = I.x;
          w2.cr[i].dy = I.y;


      }



    }

    return 0; // rotation cycle not over
  }




  // creates dump of game map according to:
  //    game map:   map, mapw, maph -- mat2, width, height
  //    wall:       w               -- w1 or w2
  //    direction:  d               -- 0:up, 1:right, 2:down, 3:left
  //    rotation:   r               -- +1:left, -1:right
  //    position:   x, y            -- unsigned int
  int dump_map(const int map[][MAP_SIZE], int mapw, int maph, WALL& w, int d, int r, int x, int y) {
    int mmx = x;
    int mmy = y;
    int dx1 = 0;
    int dy1 = 0;
    int dx2 = 0;
    int dy2 = 0;
    switch(d) {
      case 0: mmx -= r; mmy -= 0; dy1 = -1; dx2 =  r; break;
      case 1: mmx += 0; mmy -= r; dx1 =  1; dy2 =  r; break;
      case 2: mmx += r; mmy += 0; dy1 =  1; dx2 = -r; break;
      case 3: mmx -= 0; mmy += r; dx1 = -1; dy2 = -r; break;
    }
    for (int i = 0; i < 3; ++i) {
      int x_ = mmx;
      int y_ = mmy;
      for (int c = 0; c < cn; ++c) {
        w.ma[i][c] = 2; // default
        if (x_ >= 0 && x_ < mapw && y_ >= 0 && y_ < maph) {
          w.ma[i][c] = map[x_][y_];
        }
        x_ += dx1;
        y_ += dy1;
      }
      mmx += dx2;
      mmy += dy2;
    }
  }




  int copy_map() {
    for (int y = 0; y < 3; ++y)
      for (int x = 0; x < cn; ++x)
        w1.ma[y][x] = w2.ma[2 - y][x];
  }





  // randomly rotates camera
  int next_next(int& __rot, int& __dir, int& __flg) {
    // possible directions

    //console.log(px, py);

    int d = ((map[px + 0][py - 1] == 0) << 0) | // ^
              ((map[px + 1][py + 0] == 0) << 1) | // >
              ((map[px + 0][py + 1] == 0) << 2) | // v
              ((map[px - 1][py + 0] == 0) << 3) ; // <

    int b = (dir + 2) % 4;
    int nd = dir + (rand() % 4);
    for (int i = 0; i < 4; ++i) {
      nd %= 4;
      if (nd != b && ((d >> nd) & 1)) {
        __rot = (((nd + 1) % 4) == dir) ? +1 : -1;
        __dir = nd;
        __flg = F_ROT;
        return 0;
      }
      ++nd;
    }

    __rot = (rand() % 2 == 0) ? -1 : 1;
    __dir = dir - 1 < 0 ? 3 : dir - 1;
    __flg = F_AHD;
    return 0;
  }

/*
  function draw_line(x1, y1, x2, y2, depth) {
    ctx.strokeStyle = colors[depth];
    ctx.lineWidth = 2;
    ctx.beginPath();
    ctx.moveTo(x1, y1);
    ctx.lineTo(x2, y2);
    ctx.stroke();
  }
*/


  int demo_init() {

    dump_map(map, MAP_SIZE, MAP_SIZE, w1, dir, rot, px, py);

  }






  int init() {
    cx_init();
    w_init();
    demo_init();
  }


  int deinit() {

  }





  // demoscene


  int demo() {

    // camera in cell (waits for new command)
    if (F_PRC == flg) {
      int next_rot, next_dir, next_flg;
      next_next(next_rot, next_dir, next_flg);
      if (dir == next_dir) {
        flg = F_MOV;
      } else {
        rot = next_rot;
        dump_map(map, MAP_SIZE, MAP_SIZE, w1, dir, rot, px, py);
        dir = next_dir;
        dump_map(map, MAP_SIZE, MAP_SIZE, w2, dir, rot, px, py);
        flg = next_flg;
      }
    }

    // camera rotating
    if (F_ROT == flg) {
      if (1 == rotate(rot)) {
        rot = 1;
        dump_map(map, MAP_SIZE, MAP_SIZE, w1, dir, rot, px, py);
        flg = ROT_NEXT;
      }
    }

    // camera moving
    if (F_MOV == flg) {
      if (1 == move()) {
        switch (dir) {
          case 0: py--; break;
          case 1: px++; break;
          case 2: py++; break;
          case 3: px--; break;
        }
        dump_map(map, MAP_SIZE, MAP_SIZE, w1, dir, rot, px, py);
        flg = MOV_NEXT;
      }
    }

    // camera in deadend (must rotate 2 times)
    if (F_AHD == flg) {
      if (1 == rotate(rot)) {
        dump_map(map, MAP_SIZE, MAP_SIZE, w1, dir, rot, px, py);
        dir = dir - 1 < 0 ? 3 : dir - 1;
        dump_map(map, MAP_SIZE, MAP_SIZE, w2, dir, rot, px, py);
        flg = F_ROT;
      }
    }

    // drawing
    //ctx.fillStyle = 'black';
    //ctx.fillRect(0, 0, ctx.canvas.width, ctx.canvas.height);
    draw_wall(w1);
    if (F_ROT == flg) {
      draw_wall(w2);
    }
     /*
    ctx.fillStyle = 'white';
    for (let y = 0; y < 16; ++y)
      for (let x = 0; x < 16; ++x)
        if (map[x][y])
          ctx.fillRect(10 * x, 10 * (16 - y - 2), 10, 10);
    ctx.fillStyle = 'red';
    ctx.fillRect(10 * px, 10 * (16 - py - 2), 10, 10);
    */

    minimap();
    dumpmap(w1, 10);
    dumpmap(w2, 100);
  }



}

#endif // GAME_H
