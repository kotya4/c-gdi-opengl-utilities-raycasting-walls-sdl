#ifndef GAME_H
#define GAME_H

namespace game {

  // options

  int DisplayWidth  = 100; // (wx) // window width (can be changed in runtime without harm)
  int DisplayHeight = 100; // (wy) window height

  int ColumnsNumber = 7; // (cn) // columns number (CANNOT BE LESS THAN 3 AND MORE THAN 8 * sizeof(wx), see cx initialization)
  int DetailsSkip = 0; // (details_reduction) // skips column details from further (zero or more)

  // constants

  float* ColumnsX; // (cx) // default (constant) column x positions (see init), because Y's not changing, but X's do.
  float* ColumnsSpeed; // (w1/w2.cl/cr[i].sx/sy) // speed of each column, [sx1, sy1, ..., sxn, syn]

  float RotationSpeed = DisplayWidth / 50.f; // (rs) // rotation speed (pixel per frame)
  float MovingSpeed = 50.f; // (ms) // move speed (frame per second)
  float AccDelta = (float)DisplayWidth / DisplayHeight / MovingSpeed; // (ad) // acceleration delta
  float MiddotSpeed = -RotationSpeed*DisplayHeight/DisplayWidth/2.f; // (w1.pa) // speed of horizont point (reversed for w2)

  // vars changing in functions

  float Acc = 1.f; // (ac) // current acceleration
  int Status = 0; // (st) // drawing status
  int MirrorX = +1; // (mx) // mirror (+1:no, -1:yes)
  float OffsetX = 0.f; // (dx) // offset (changes on rotation)

  int init();
  int deinit();




  int define_wall(middot_speed) {
    int* mapdump; // (ma) // visible part of map (dumped map) 3 x ColumnsNumber

    float *colLpos;
    float *colRpos;



    // Nested array of columns looks like:
    //   column position    door position    subcolumn position   subcolumn speed
    // [ Cx0,Cy0,           Dx0,Dy0,       [ Sx0,Sy0,             Ssx0, Ssy0,       ..., ]
    //       ...,
    //   xN,yN,           dxN,dyN, ]
    float* L; // columns by left
    float* R; // columns by right

    return {
      ma: [...Array(3)].map(_ => [...Array(cn)].fill(0)), // visible part of map (dumped map)
      pa: middot_speed,
      // TODO: do not define last columns subcolumns
      cl: [...Array(cn)].map(_ => ({ subcolumns: [...Array(subcolumns_num)].map(_ => ({ points: [...Array(subrows_num_HALF)].map(_ => ({})) })) })), // columns by left
      cr: [...Array(cn)].map(_ => ({ subcolumns: [...Array(subcolumns_num)].map(_ => ({ points: [...Array(subrows_num_HALF)].map(_ => ({})) })) }))  // columns by right
      // Some properties defined in 'reset_main_wall' and 'reset_side_wall'.
    };
  }












  int reset_primary_wall() {
    w1.lx = 0;      // position left
    w1.ly = 0;
    w1.rx = wx;     // position right
    w1.ry = 0;
    w1.ox = wx / 2; // position center
    w1.oy = wy / 2;
    w1.px = wx / 2; // pointer (middot)
    w1.py = wy / 2;

    for (let i = 0; i < cn; ++i) {
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

      if (i + 1 > cn - 1) break; // last column subcolumns doesn't exists

      let column_w = cx[i + 1] - cx[i];
      const subcolumn_w = column_w / (subcolumns_num + 1);

      let acc_x = cx[i]; // default previous x position (accumulator)


      for (let k = 0; k < subcolumns_num; ++k) {
        // left subcolumns
        const sL = w1.cl[i].subcolumns[k];
        acc_x += subcolumn_w;
        sL.x  = acc_x;
        // TIP: You also can do this:
        //      `if (k === 0) sL.x += subcolumn_offset;`
        //      where `subcolumn_offset = subcolumn_w * DELTA;`, DELTA can be [0; 1).
        //      Adds offset to subcolumns and makes they more perspective.
        sL.y  = sL.x * wy / wx; // y position

        sL.sx = i > 0 ? ((w1.cl[i - 1].subcolumns[k].x - sL.x) / ms) : 0;

        sL.sy = sL.sx * wy / wx;

        // right subcolumns
        const sR = w1.cr[i].subcolumns[k];
        sR.x = -sL.x;
        sR.y = +sL.y;
        // As right columns, right subcolumns uses
        // left subcolumn speed parameters to move.
        // No need to define sx and sy.

        // points (p.x ALWAYS same as sL.x)

        const sLPh = (wy - sL.y * 2) / subrows_num; // left subcolumn points height (sLPh same as sRPh)

        for (let t = 0; t < subrows_num_HALF; ++t) {
          const pL = sL.points[t];
          pL.y  = sL.y + sLPh * (t + 1);
          pL.sy = i > 0 ? ((w1.cl[i - 1].subcolumns[k].points[t].y - pL.y) / ms) : 0;

          sR.points[t].y  = +pL.y;
          sR.points[t].sy = +pL.sy; // no need to define?
        }
      }
    }
  }




























  int init() {

    ColumnsX = new int [ColumnsNumber];
    float offset = DisplayWidth / 2.f;
    ColumnsX[0] = -offset;
    ColumnsX[1] = 0.f;
    ColumnsX[ColumnsNumber-1] = +offset;
    for (int i = 2; i < ColumnsNumber-1; ++i)
      ColumnsX[i] = ColumnsX[i-1] + (offset /= 2.f);

    const w1 = define_wall(-RotationSpeed*DisplayHeight/DisplayWidth/2.f);
    const w2 = define_wall(-w1.pa);
    reset_main_wall(w1);
    reset_side_wall(w2, w1);




  }

  int deinit() {

    delete [] ColumnsX;

  }
























}

#endif // GAME_H
