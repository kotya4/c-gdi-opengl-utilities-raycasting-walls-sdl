#include <cstdio>
#include <cstdlib>
#include <conio.h>
#include <windows.h>


enum COLOR_16 {
  BLACK, RED, GREEN, YELLOW,
  BLUE, MAGENTA, CYAN, WHITE
};


void set_color_16() {
  printf("\033[0m");
}


void set_color_16(unsigned char color) {
  unsigned char fg = color & 0xf;
  unsigned char bg = color >> 4;
  printf("\033[38;5;%dm\033[48;5;%dm", fg, bg);
}


void set_color_16_win(unsigned char color = 0x07) {
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}



typedef struct BMPOBJ {
  int size;
  int width;
  int height;
  unsigned char info[54];
  unsigned char *data;
} BMPOBJ;

BMPOBJ readBMP(const char* filename) {
  BMPOBJ bmpobj;
  
  FILE* f = fopen(filename, "rb");  
  fread(bmpobj.info, sizeof(unsigned char), 54, f); // read the 54-byte header

  bmpobj.width = *(int*)&bmpobj.info[18];
  bmpobj.height = *(int*)&bmpobj.info[22]; // TIP: can be -1
  bmpobj.size = 3 * bmpobj.width * bmpobj.height; // allocate 3 bytes per pixel
  
  bmpobj.data = new unsigned char[bmpobj.size]; // BGR
  fread(bmpobj.data, sizeof(unsigned char), bmpobj.size, f); // read the rest of the data at once
  fclose(f);

  return bmpobj;
}



typedef struct RgbColor { unsigned char r, g, b; } RgbColor;
typedef struct HsvColor { unsigned char h, s, v; } HsvColor;

HsvColor RgbToHsv(RgbColor rgb)
{
    HsvColor hsv;
    unsigned char rgbMin, rgbMax;

    rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
    rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);

    hsv.v = rgbMax;
    if (hsv.v == 0)
    {
        hsv.h = 0;
        hsv.s = 0;
        return hsv;
    }

    hsv.s = 255 * ((long)(rgbMax - rgbMin)) / hsv.v;
    if (hsv.s == 0)
    {
        hsv.h = 0;
        return hsv;
    }

    if (rgbMax == rgb.r)
        hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
    else if (rgbMax == rgb.g)
        hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
    else
        hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);

    return hsv;
}




int hsv_to_16(HsvColor &hsv, float *pcolors, const int PCOLORS_NUM) {
  int color;
  
  if (hsv.s < 40) {
    if      (hsv.v <  65) color = BLACK;
    else if (hsv.v < 130) color = BLACK | 0x08;
    else if (hsv.v < 200) color = WHITE;
    else                  color = WHITE | 0x08;
  } else {
    int pcolor_hue = hsv.h;
    int pcolor_index;
    
    for (pcolor_index = 0; pcolor_index < PCOLORS_NUM; ++pcolor_index) {
      if ((pcolor_hue -= pcolors[pcolor_index]) <= 0) break;
    }
    
    switch(pcolor_index) {
      case 0: color =     RED; break;
      case 1: color =  YELLOW; break;
      case 2: color =   GREEN; break;
      case 3: color =    CYAN; break;
      case 4: color =    BLUE; break;
      case 5: color = MAGENTA; break;
      case 6: color =     RED; break;
    };
    
    if ((hsv.s + hsv.v >> 1) > 200) color = color | 0x08;
  }

  if (hsv.v < 40) color = BLACK;
  
  return color;
}



typedef struct Vec3i { int x, y, z; } Vec3i;

Vec3i hsv_fade(HsvColor &hsv, float *fpcolors, const int FPCOLORS_NUM) {
  Vec3i symbol;
  
 
  int fpcolor_hue = hsv.h;
  int fpcolor_index;
  
  for (fpcolor_index = 0; fpcolor_index < FPCOLORS_NUM; ++fpcolor_index) {
    if ((fpcolor_hue -= fpcolors[fpcolor_index]) <= 0) break;
  }
  
  fpcolor_index %= 24;
  
  int ch = fpcolor_index % 4;
  int co = fpcolor_index / 4;
  
  int color;
  
  switch(co) {
    case 0: color =     RED; break;
    case 1: color =  YELLOW; break;
    case 2: color =   GREEN; break;
    case 3: color =    CYAN; break;
    case 4: color =    BLUE; break;
    case 5: color = MAGENTA; break;
  };
  symbol.x = color;
  
  switch(co) {
    case 5: color =     RED; break;
    case 0: color =  YELLOW; break;
    case 1: color =   GREEN; break;
    case 2: color =    CYAN; break;
    case 3: color =    BLUE; break;
    case 4: color = MAGENTA; break;
  };
  symbol.y = color;
  
  symbol.z = ch;
  
  /*
  // more whity
  const int so = 90;
  if (hsv.s < so) {
    
    symbol.z = 3 - hsv.s / (so / 4);
    

    if (symbol.z < 3) symbol.z += 1;
    else symbol.z = 0;
    
    symbol.x = WHITE | 0x8;
    
    
  }
  
  // more darky
  const int vo = 100;
  if (hsv.v < vo) {
    
    symbol.z = 3 - hsv.v / (vo / 4);
    

    if (symbol.z < 3) symbol.z += 1;
    else {
      symbol.z = 0;
      symbol.x = BLACK;
    }
    
    symbol.y = BLACK;
  }
  */
  
  
  
  
  
  return symbol;
}







int main() {
/*
  for (int i = 0; i < 256; ++i) {
    printf("%c %i ; ", i, i);
  }
*/ 
  BMPOBJ bmp = readBMP("b.bmp");
  
  
  const int PCOLORS_NUM = 7;
  float pcolors [PCOLORS_NUM] = {
    /* red */ 10,
    /* yel */ 20,
    /* gre */ 20,
    /* cya */ 20,
    /* blu */ 20,
    /* mag */ 20,
    /* red */ 15
  };
  float pcolors_sum = 0;
  for (int i = 0; i < PCOLORS_NUM; ++i) pcolors_sum += pcolors[i];
  for (int i = 0; i < PCOLORS_NUM; ++i) pcolors[i] = pcolors[i] * 256 / pcolors_sum;
  
  
  
  
  
  
  const int FPCOLORS_NUM = 25;
  float fpcolors [FPCOLORS_NUM] = {
    /* red */ 10,
    /* re1 */ 20,
    /* re2 */ 20,
    /* re3 */ 20,
    /* yel */ 20,
    /* ye1 */ 20,
    /* ye2 */ 20,
    /* ye3 */ 20,
    /* gre */ 20,
    /* gr1 */ 20,
    /* gr2 */ 20,
    /* gr3 */ 20,
    /* cya */ 20,
    /* cy1 */ 20,
    /* cy2 */ 20,
    /* cy3 */ 20,
    /* blu */ 20,
    /* bl1 */ 20,
    /* bl2 */ 20,
    /* bl3 */ 20,
    /* mag */ 20,
    /* ma1 */ 20,
    /* ma2 */ 20,
    /* ma3 */ 20,
    /* red */ 15
  };
  float fpcolors_sum = 0;
  for (int i = 0; i < FPCOLORS_NUM; ++i) fpcolors_sum += fpcolors[i];
  for (int i = 0; i < FPCOLORS_NUM; ++i) fpcolors[i] = fpcolors[i] * 256 / fpcolors_sum;
  
  
  
  

  for (int y = 0; y < bmp.height; y += 2) { // 1 character per 2 pixels
    for (int x = 0; x < bmp.width; ++x) {
      
      int pos1 = 3 * (x + (bmp.height - (y + 0) - 1) * bmp.width);
      int pos2 = 3 * (x + (bmp.height - (y + 1) - 1) * bmp.width);
      
      if (y >= bmp.height) pos2 = pos1;
      
      RgbColor rgb1 = { bmp.data[2 + pos1], bmp.data[1 + pos1], bmp.data[0 + pos1] };
      RgbColor rgb2 = { bmp.data[2 + pos2], bmp.data[1 + pos2], bmp.data[0 + pos2] };
      
      HsvColor hsv1 = RgbToHsv(rgb1);
      HsvColor hsv2 = RgbToHsv(rgb2);
      
      int color1 = hsv_to_16(hsv1, pcolors, PCOLORS_NUM);
      int color2 = hsv_to_16(hsv2, pcolors, PCOLORS_NUM);
      
      int br1 = color1 & 0x08;
      int br2 = color2 & 0x08;
      
      if (color1 != color2) {
        set_color_16((color1 << 4) | (color2 & 0x0f));
        printf("%c", 220);
      } else {
        HsvColor hsv = {
          hsv1.h + hsv2.h >> 1,
          hsv1.s + hsv2.s >> 1,
          hsv1.v + hsv2.v >> 1
        };
       
        Vec3i s = hsv_fade(hsv, fpcolors, FPCOLORS_NUM);
        
        
        
        set_color_16(((s.x | br1) << 4) | ((s.y | br2) & 0x0f));
        if (s.z > 0) printf("%c", 176 + s.z - 1); else printf(" ");
        
        //set_color_16();
        //printf("%d", s.z);
        
      }
      
      
      
      
      //color <<= 4;
      
      // BLACK, RED, GREEN, YELLOW,
      // BLUE, MAGENTA, CYAN, WHITE
      
      // _   _
      // V + S  =  8th bit
      // _
      // H      =  another 7
      //
      
      //int VS = (hsv1.v + hsv1.s) / 2;
      
      
      // color = (hsv.h & 0xf0) | (hsv.v >> 4);
      // color |= (hsv.v >> 4);
      // if (color == 0) color = 4;
      
      
      
    }
    printf("\n");
  }
  
  set_color_16();
  
  set_color_16(0x0f);
  printf(" ");
  for (int i = 0; i < 3; ++i) {
    printf("%c", 176 + i);
  }
  set_color_16(0xf0);
  printf(" \n");
  
  set_color_16(0x07);
  printf(" ");
  for (int i = 0; i < 3; ++i) {
    printf("%c", 176 + i);
  }
  set_color_16(0x70);
  printf(" \n");
  
  set_color_16(0x08);
  printf(" ");
  for (int i = 0; i < 3; ++i) {
    printf("%c", 176 + i);
  }
  set_color_16(0x80);
  printf(" \n");
  

  
  
  
  
/*  
  const int MAP_WIDTH = 9;
  const int MAP_HEIGHT = 5;
  unsigned char map [MAP_WIDTH * MAP_HEIGHT];
  

  for (int y = 0; y < MAP_HEIGHT; ++y) {
    for (int x = 0; x < MAP_WIDTH; ++x) {
      
      map[x + y * MAP_WIDTH] = rand() % 2 ? 127 : 0x20;
      
    }
    
    map[MAP_WIDTH - 1 + y * MAP_WIDTH] = '\n';
  }
  
  map[MAP_WIDTH * MAP_HEIGHT - 1] = 0;
  
  printf("\033[31;47m%s\033]", map);
*/

  // printf("hellow, world!");
  set_color_16();
}
