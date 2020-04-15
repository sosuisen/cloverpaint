#include "clover.h"


Palette::Palette(Canvas * parent, const char * name, bool modal, WFlags f)
  :QDialog(parent, name, modal, f)
{
  canvas = parent;

  moveFlag = false;
  stickyFlag = false;
  minimizePressFlag = false;
  closePressFlag = false;
  display = false;
  reparent( parent,  QPoint(PALETTEPANEL_X, canvas->palettePanelY));
  resize(65, 201);

  //xpmからの読みこみはインデックスカラーになるため変換
  paletteImg = new QImage(*(canvas->palette_normalImg));

  //ツールバー画像の表示
  QPixmap *palettePix = new QPixmap();
  palettePix->convertFromImage(*paletteImg, 0);
  setBackgroundPixmap(*palettePix);
  delete palettePix;

  paletteSpoit = true;

  myReset();
}

Palette::~Palette()
{
  delete paletteImg;
}

void Palette::showEvent( QShowEvent *e ){
  this->setActiveWindow();
  this->raise();

  if(!stickyFlag && (this->y() != canvas->palettePanelY)){
    move(PALETTEPANEL_X, canvas->palettePanelY);
  }

  myReset();

}

void Palette::myReset(){

  int left2=0;
  int top2=0;

  if(paletteSpoit){
    left2 = 32;
    top2 = 168;
  }
  else{
    left2 = 3;
    top2 = 168;	  
  }

  *paletteImg = canvas->palette_normalImg->copy();

  for(int y=top2; y<top2+29; y++){
    for(int x=left2; x<left2+29; x++){
      paletteImg->setPixel(x,y,canvas->palette_reverseImg->pixel(x,y));
    }
  }

  for(int y=22; y<22+25; y++){
    for(int x=5; x<5+25; x++){
      int tmpCol = 0;
      if(canvas->useColorMatrix){
	tmpCol = (( canvas->colorMatrix[0][((canvas->colorPalette[0] & 0x00ff0000)>>16) & 0x000000ff] << 16) & 0x00ff0000 )
	  | (( canvas->colorMatrix[1][((canvas->colorPalette[0] & 0x0000ff00)>>8) & 0x000000ff] << 8) & 0x0000ff00)
	  | (( canvas->colorMatrix[2][((canvas->colorPalette[0] & 0x000000ff)) & 0x000000ff] ) & 0x000000ff)
	  | 0xff000000;
      }
      else{
	tmpCol = canvas->colorPalette[0];
      }
      paletteImg->setPixel(x,y,tmpCol);
    }
  }
  for(int y=51; y<51+25; y++){
    for(int x=5; x<5+25; x++){
      int tmpCol = 0;
      if(canvas->useColorMatrix){
	tmpCol = (( canvas->colorMatrix[0][((canvas->colorPalette[1] & 0x00ff0000)>>16) & 0x000000ff] << 16) & 0x00ff0000 )
	  | (( canvas->colorMatrix[1][((canvas->colorPalette[1] & 0x0000ff00)>>8) & 0x000000ff] << 8) & 0x0000ff00)
	  | (( canvas->colorMatrix[2][((canvas->colorPalette[1] & 0x000000ff)) & 0x000000ff] ) & 0x000000ff)
	  | 0xff000000;
      }
      else{
	tmpCol = canvas->colorPalette[1];
      }
      paletteImg->setPixel(x,y,tmpCol);
    }
  }
  for(int y=80; y<80+25; y++){
    for(int x=5; x<5+25; x++){
      int tmpCol = 0;
      if(canvas->useColorMatrix){
	tmpCol = (( canvas->colorMatrix[0][((canvas->colorPalette[2] & 0x00ff0000)>>16) & 0x000000ff] << 16) & 0x00ff0000 )
	  | (( canvas->colorMatrix[1][((canvas->colorPalette[2] & 0x0000ff00)>>8) & 0x000000ff] << 8) & 0x0000ff00)
	  | (( canvas->colorMatrix[2][((canvas->colorPalette[2] & 0x000000ff)) & 0x000000ff] ) & 0x000000ff)
	  | 0xff000000;
      }
      else{
	tmpCol = canvas->colorPalette[2];
      }
      paletteImg->setPixel(x,y,tmpCol);

    }
  }
  for(int y=109; y<109+25; y++){
    for(int x=5; x<5+25; x++){
      int tmpCol = 0;
      if(canvas->useColorMatrix){
	tmpCol = (( canvas->colorMatrix[0][((canvas->colorPalette[3] & 0x00ff0000)>>16) & 0x000000ff] << 16) & 0x00ff0000 )
	  | (( canvas->colorMatrix[1][((canvas->colorPalette[3] & 0x0000ff00)>>8) & 0x000000ff] << 8) & 0x0000ff00)
	  | (( canvas->colorMatrix[2][((canvas->colorPalette[3] & 0x000000ff)) & 0x000000ff] ) & 0x000000ff)
	  | 0xff000000;
      }
      else{
	tmpCol = canvas->colorPalette[3];
      }
      paletteImg->setPixel(x,y,tmpCol);

    }
  }
  for(int y=138; y<138+25; y++){
    for(int x=5; x<5+25; x++){
      int tmpCol = 0;
      if(canvas->useColorMatrix){
	tmpCol = (( canvas->colorMatrix[0][((canvas->colorPalette[4] & 0x00ff0000)>>16) & 0x000000ff] << 16) & 0x00ff0000 )
	  | (( canvas->colorMatrix[1][((canvas->colorPalette[4] & 0x0000ff00)>>8) & 0x000000ff] << 8) & 0x0000ff00)
	  | (( canvas->colorMatrix[2][((canvas->colorPalette[4] & 0x000000ff)) & 0x000000ff] ) & 0x000000ff)
	  | 0xff000000;
      }
      else{
	tmpCol = canvas->colorPalette[4];
      }
      paletteImg->setPixel(x,y,tmpCol);

    }
  }

  for(int y=22; y<22+25; y++){
    for(int x=34; x<34+25; x++){
      int tmpCol = 0;
      if(canvas->useColorMatrix){
	tmpCol = (( canvas->colorMatrix[0][((canvas->colorPalette[5] & 0x00ff0000)>>16) & 0x000000ff] << 16) & 0x00ff0000 )
	  | (( canvas->colorMatrix[1][((canvas->colorPalette[5] & 0x0000ff00)>>8) & 0x000000ff] << 8) & 0x0000ff00)
	  | (( canvas->colorMatrix[2][((canvas->colorPalette[5] & 0x000000ff)) & 0x000000ff] ) & 0x000000ff)
	  | 0xff000000;
      }
      else{
	tmpCol = canvas->colorPalette[5];
      }
      paletteImg->setPixel(x,y,tmpCol);

    }
  }
  for(int y=51; y<51+25; y++){
    for(int x=34; x<34+25; x++){
      int tmpCol = 0;
      if(canvas->useColorMatrix){
	tmpCol = (( canvas->colorMatrix[0][((canvas->colorPalette[6] & 0x00ff0000)>>16) & 0x000000ff] << 16) & 0x00ff0000 )
	  | (( canvas->colorMatrix[1][((canvas->colorPalette[6] & 0x0000ff00)>>8) & 0x000000ff] << 8) & 0x0000ff00)
	  | (( canvas->colorMatrix[2][((canvas->colorPalette[6] & 0x000000ff)) & 0x000000ff] ) & 0x000000ff)
	  | 0xff000000;
      }
      else{
	tmpCol = canvas->colorPalette[6];
      }
      paletteImg->setPixel(x,y,tmpCol);

    }
  }
  for(int y=80; y<80+25; y++){
    for(int x=34; x<34+25; x++){
      int tmpCol = 0;
      if(canvas->useColorMatrix){
	tmpCol = (( canvas->colorMatrix[0][((canvas->colorPalette[7] & 0x00ff0000)>>16) & 0x000000ff] << 16) & 0x00ff0000 )
	  | (( canvas->colorMatrix[1][((canvas->colorPalette[7] & 0x0000ff00)>>8) & 0x000000ff] << 8) & 0x0000ff00)
	  | (( canvas->colorMatrix[2][((canvas->colorPalette[7] & 0x000000ff)) & 0x000000ff] ) & 0x000000ff)
	  | 0xff000000;
      }
      else{
	tmpCol = canvas->colorPalette[7];
      }
      paletteImg->setPixel(x,y,tmpCol);

    }
  }
  for(int y=109; y<109+25; y++){
    for(int x=34; x<34+25; x++){
      int tmpCol = 0;
      if(canvas->useColorMatrix){
	tmpCol = (( canvas->colorMatrix[0][((canvas->colorPalette[8] & 0x00ff0000)>>16) & 0x000000ff] << 16) & 0x00ff0000 )
	  | (( canvas->colorMatrix[1][((canvas->colorPalette[8] & 0x0000ff00)>>8) & 0x000000ff] << 8) & 0x0000ff00)
	  | (( canvas->colorMatrix[2][((canvas->colorPalette[8] & 0x000000ff)) & 0x000000ff] ) & 0x000000ff)
	  | 0xff000000;
      }
      else{
	tmpCol = canvas->colorPalette[8];
      }
      paletteImg->setPixel(x,y,tmpCol);

    }
  }
  for(int y=138; y<138+25; y++){
    for(int x=34; x<34+25; x++){
      int tmpCol = 0;
      if(canvas->useColorMatrix){
	tmpCol = (( canvas->colorMatrix[0][((canvas->colorPalette[9] & 0x00ff0000)>>16) & 0x000000ff] << 16) & 0x00ff0000 )
	  | (( canvas->colorMatrix[1][((canvas->colorPalette[9] & 0x0000ff00)>>8) & 0x000000ff] << 8) & 0x0000ff00)
	  | (( canvas->colorMatrix[2][((canvas->colorPalette[9] & 0x000000ff)) & 0x000000ff] ) & 0x000000ff)
	  | 0xff000000;
      }
      else{
	tmpCol = canvas->colorPalette[9];
      }
      paletteImg->setPixel(x,y,tmpCol);

    }
  }





  if(stickyFlag){
    for(int y=3; y<17; y++){
      for(int x=32; x<61; x++){
	paletteImg->setPixel(x,y,canvas->palette_reverseImg->pixel(x,y));
      }
    }
  }
  else{
    for(int y=3; y<17; y++){
      for(int x=32; x<61; x++){
	paletteImg->setPixel(x,y,canvas->palette_normalImg->pixel(x,y));
      }
    }
  }


    if(minimizePressFlag){
      for(int y=3; y<17; y++){
	for(int x=32; x<46; x++){
	  paletteImg->setPixel(x,y,(~(canvas->palette_reverseImg->pixel(x,y))) | 0xff000000);
	}
      }
    }
    else{
      if(stickyFlag){
	for(int y=3; y<17; y++){
	  for(int x=32; x<46; x++){
	    paletteImg->setPixel(x,y,((canvas->palette_reverseImg->pixel(x,y))) | 0xff000000);
	  }
	}
      }
      else{
	for(int y=3; y<17; y++){
	  for(int x=32; x<46; x++){
	    paletteImg->setPixel(x,y,((canvas->palette_normalImg->pixel(x,y))) | 0xff000000);
	  }
	}
      }
    }

    if(closePressFlag){
      for(int y=3; y<17; y++){
	for(int x=47; x<61; x++){
	  paletteImg->setPixel(x,y,(~(canvas->palette_reverseImg->pixel(x,y))) | 0xff000000);
	}
      }
    }
    else{
      if(stickyFlag){
	for(int y=3; y<17; y++){
	  for(int x=47; x<61; x++){
	    paletteImg->setPixel(x,y,((canvas->palette_reverseImg->pixel(x,y))) | 0xff000000);
	  }
	}
      }
      else{
	for(int y=3; y<17; y++){
	  for(int x=47; x<61; x++){
	    paletteImg->setPixel(x,y,((canvas->palette_normalImg->pixel(x,y))) | 0xff000000);
	  }
	}
      }
    }


  QPixmap *palettePix = new QPixmap();
  palettePix->convertFromImage(*paletteImg, 0);
  setBackgroundPixmap(*palettePix);
  delete palettePix;


}


void Palette::mousePressEvent( QMouseEvent *e )
{

  minimizePressFlag = false;
  closePressFlag = false;

  if(!this->isTopLevel()){
    this->setActiveWindow();
    this->raise();
  }

  if(e->y() >= 0 && e->y() < 17){
    if(e->x() >= 0 && e->x() < 32){

      prev_x = e->x();
      prev_y = e->y();

      moveFlag = true;
      stickyFlag = true;

      myReset();
      return;
    }
  }

  if(e->y() >= 0 && e->y() < 17){
    if(e->x() >= 32 && e->x() < 46){
      minimizePressFlag = true;
      myReset();
      return;
    }
    else if(e->x() >= 47 && e->x() < 61){
      closePressFlag = true;
      myReset();
      return;
    }
  }

  int colIndex = -1;
  if(e->x() >= 3 && e->x() < 32){
    // 左列
    if(e->y() >= 20 && e->y() < 49){
      colIndex = 0;
    }
    else if(e->y() >= 49 && e->y() < 78){
      colIndex = 1;
    }
    else if(e->y() >= 78 && e->y()< 107){
      colIndex = 2;
    }
    else if(e->y() >= 107 && e->y()< 136){
      colIndex = 3;
    }
    else if(e->y() >= 136 && e->y()< 165){
      colIndex = 4;
    }
    else if(e->y() >= 168 && e->y()< 197){
      paletteSpoit = false;
      myReset();
    }

  }
  else if (e->x()>= 32 && e->x()< 61){
    // 右列
    if(e->y() >= 20 && e->y() < 49){
      colIndex = 5;
    }
    else if(e->y() >= 49 && e->y() < 78){
      colIndex = 6;
    }
    else if(e->y() >= 78 && e->y()< 107){
      colIndex = 7;
    }    
    else if(e->y() >= 107 && e->y()< 136){
      colIndex = 8;
    }    
    else if(e->y() >= 136 && e->y()< 165){
      colIndex = 9;
    }    
    else if(e->y() >= 168 && e->y()< 197){
      paletteSpoit = true;
      myReset();
    }    
  }

  if(paletteSpoit){
    if(colIndex != -1){
      
      canvas->toolbar->reverseSBCursor();
      canvas->pencolor = canvas->colorPalette[colIndex];

      int tmphue = canvas->fgHu;
      canvas->RGB2HSB();
      int tmphue2 = canvas->fgHu;
      canvas->fgHu = tmphue;
      canvas->toolbar->reverseSBCursor();
      canvas->huebar->changeHueArea(COLORDIV-tmphue2, -1);
      canvas->toolbar->changeSBArea();
      canvas->toolbar->changeColorArea();

      canvas->toolbar->repaint_left = SBAREA_X-2;
      canvas->toolbar->repaint_top = SBAREA_Y-2;
      canvas->toolbar->repaint_width = SBAREA_WIDTH+4;
      canvas->toolbar->repaint_height = SBAREA_HEIGHT+4;
      QRegion r(canvas->toolbar->repaint_left,canvas->toolbar->repaint_top,canvas->toolbar->repaint_width,canvas->toolbar->repaint_height,QRegion::Rectangle);
      canvas->toolbar->repaint(r, false);

      canvas->toolbar->repaint_left = COLORAREA_X;
      canvas->toolbar->repaint_top = COLORAREA_Y;
      canvas->toolbar->repaint_width = COLORAREA_WIDTH;
      canvas->toolbar->repaint_height = COLORAREA_HEIGHT;
      r =QRegion(canvas->toolbar->repaint_left,canvas->toolbar->repaint_top,canvas->toolbar->repaint_width,canvas->toolbar->repaint_height,QRegion::Rectangle);
      canvas->toolbar->repaint(r, false);

      canvas->toolbar->repaint_left = COLORAREA3_X;
      canvas->toolbar->repaint_top = COLORAREA3_Y;
      canvas->toolbar->repaint_width = COLORAREA3_WIDTH;
      canvas->toolbar->repaint_height = COLORAREA3_HEIGHT;
      r =QRegion(canvas->toolbar->repaint_left,canvas->toolbar->repaint_top,canvas->toolbar->repaint_width,canvas->toolbar->repaint_height,QRegion::Rectangle);
      canvas->toolbar->repaint(r, false);

      int repaint_left;
      int repaint_top;
      int repaint_width;
      int repaint_height;

      repaint_left = 479-(int)(479*canvas->fgHu/COLORDIV)-1;
      if(repaint_left < 0){
	repaint_left = 0;
      }
      else if(repaint_left > 477){
	repaint_left = 477;
      }
      repaint_top = 0;
      repaint_width = 3;
      repaint_height = 20;
      canvas->toolbar->repaintColor(repaint_left, repaint_top, repaint_width, repaint_height, false);

      return;

    }

  }


}


void Palette::mouseMoveEvent( QMouseEvent *e )
{

  int next_x, next_y;
  minimizePressFlag = false;
  closePressFlag = false;

  if(moveFlag){
    next_x = x()+e->x()-prev_x;
    next_y = y()+e->y()-prev_y;
  
    if(next_x < 47-65){
      next_x = 47-65;
    }
    else if(next_x > 480-18){
      next_x = 480-18;
    }

    if(next_y < 0){
      next_y = 0;
    }
    else if(next_y > 640-18){
      next_y = 640-18;
    }
    move(next_x, next_y);
    myReset();
    return;
  }

  if(e->y() >= 0 && e->y() < 17){
    if(e->x() >= 32 && e->x() < 46){
      minimizePressFlag = true;
      myReset();
      return;
    }
    else if(e->x() >= 47 && e->x() < 61){
      closePressFlag = true;
      myReset();
      return;
    }
    else{
      minimizePressFlag = false;
      closePressFlag = false;
      myReset();
      return;
    }
  }


  int colIndex = -1;
  if(e->x() >= 3 && e->x() < 32){
    // 左列
    if(e->y() >= 20 && e->y() < 49){
      colIndex = 0;
    }
    else if(e->y() >= 49 && e->y() < 78){
      colIndex = 1;
    }
    else if(e->y() >= 78 && e->y()< 107){
      colIndex = 2;
    }
    else if(e->y() >= 107 && e->y()< 136){
      colIndex = 3;
    }
    else if(e->y() >= 136 && e->y()< 165){
      colIndex = 4;
    }
    else if(e->y() >= 168 && e->y()< 197){
      paletteSpoit = false;
      myReset();
    }

  }
  else if (e->x()>= 32 && e->x()< 61){
    // 右列
    if(e->y() >= 20 && e->y() < 49){
      colIndex = 5;
    }
    else if(e->y() >= 49 && e->y() < 78){
      colIndex = 6;
    }
    else if(e->y() >= 78 && e->y()< 107){
      colIndex = 7;
    }    
    else if(e->y() >= 107 && e->y()< 136){
      colIndex = 8;
    }    
    else if(e->y() >= 136 && e->y()< 165){
      colIndex = 9;
    }    
    else if(e->y() >= 168 && e->y()< 197){
      paletteSpoit = true;
      myReset();
    }    
  }


  if(paletteSpoit){
    if(colIndex != -1){
      
      canvas->toolbar->reverseSBCursor();
      canvas->pencolor = canvas->colorPalette[colIndex];

      int tmphue = canvas->fgHu;
      canvas->RGB2HSB();
      int tmphue2 = canvas->fgHu;
      canvas->fgHu = tmphue;
      canvas->toolbar->reverseSBCursor();
      canvas->huebar->changeHueArea(COLORDIV-tmphue2, -1);
      canvas->toolbar->changeSBArea();
      canvas->toolbar->changeColorArea();

      canvas->toolbar->repaint_left = SBAREA_X-2;
      canvas->toolbar->repaint_top = SBAREA_Y-2;
      canvas->toolbar->repaint_width = SBAREA_WIDTH+4;
      canvas->toolbar->repaint_height = SBAREA_HEIGHT+4;
      QRegion r(canvas->toolbar->repaint_left,canvas->toolbar->repaint_top,canvas->toolbar->repaint_width,canvas->toolbar->repaint_height,QRegion::Rectangle);
      canvas->toolbar->repaint(r, false);

      canvas->toolbar->repaint_left = COLORAREA_X;
      canvas->toolbar->repaint_top = COLORAREA_Y;
      canvas->toolbar->repaint_width = COLORAREA_WIDTH;
      canvas->toolbar->repaint_height = COLORAREA_HEIGHT;
      r =QRegion(canvas->toolbar->repaint_left,canvas->toolbar->repaint_top,canvas->toolbar->repaint_width,canvas->toolbar->repaint_height,QRegion::Rectangle);
      canvas->toolbar->repaint(r, false);

      canvas->toolbar->repaint_left = COLORAREA3_X;
      canvas->toolbar->repaint_top = COLORAREA3_Y;
      canvas->toolbar->repaint_width = COLORAREA3_WIDTH;
      canvas->toolbar->repaint_height = COLORAREA3_HEIGHT;
      r =QRegion(canvas->toolbar->repaint_left,canvas->toolbar->repaint_top,canvas->toolbar->repaint_width,canvas->toolbar->repaint_height,QRegion::Rectangle);
      canvas->toolbar->repaint(r, false);

      int repaint_left;
      int repaint_top;
      int repaint_width;
      int repaint_height;

      repaint_left = 479-(int)(479*canvas->fgHu/COLORDIV)-1;
      if(repaint_left < 0){
	repaint_left = 0;
      }
      else if(repaint_left > 477){
	repaint_left = 477;
      }
      repaint_top = 0;
      repaint_width = 3;
      repaint_height = 20;
      canvas->toolbar->repaintColor(repaint_left, repaint_top, repaint_width, repaint_height, false);

      return;

    }

  }



}



void Palette::mouseReleaseEvent( QMouseEvent *e ){


  int next_x, next_y;
  minimizePressFlag = false;
  closePressFlag = false;

  if(moveFlag){
    next_x = x()+e->x()-prev_x;
    next_y = y()+e->y()-prev_y;
  
    if(next_x < 47-65){
      next_x = 47-65;
    }
    else if(next_x > 480-18){
      next_x = 480-18;
    }

    if(next_y < 0){
      next_y = 0;
    }
    else if(next_y > 640-18){
      next_y = 640-18;
    }
    move(next_x, next_y);

    moveFlag = false;
    myReset();
    return;
  }


  //スティッキー
  if(e->x() >= 51){
    if(e->y() < 13){
      if(stickyFlag){
	stickyFlag = false;
      }
      else{
	stickyFlag = true;
      }

    }
  }



  int colIndex = -1;
  if(e->x() >= 3 && e->x() < 32){
    // 左列
    if(e->y() >= 20 && e->y() < 49){
      colIndex = 0;
    }
    else if(e->y() >= 49 && e->y() < 78){
      colIndex = 1;
    }
    else if(e->y() >= 78 && e->y()< 107){
      colIndex = 2;
    }
    else if(e->y() >= 107 && e->y()< 136){
      colIndex = 3;
    }
    else if(e->y() >= 136 && e->y()< 165){
      colIndex = 4;
    }
    else if(e->y() >= 168 && e->y()< 197){
      paletteSpoit = false;
      stickyFlag = true;
      myReset();
    }

  }
  else if (e->x()>= 32 && e->x()< 61){
    // 右列
    if(e->y() >= 20 && e->y() < 49){
      colIndex = 5;
    }
    else if(e->y() >= 49 && e->y() < 78){
      colIndex = 6;
    }
    else if(e->y() >= 78 && e->y()< 107){
      colIndex = 7;
    }    
    else if(e->y() >= 107 && e->y()< 136){
      colIndex = 8;
    }    
    else if(e->y() >= 136 && e->y()< 165){
      colIndex = 9;
    }    
    else if(e->y() >= 168 && e->y()< 197){
      paletteSpoit = true;
      myReset();
    }    
  }

  if(paletteSpoit){
    if(colIndex != -1){
      
      canvas->toolbar->reverseSBCursor();
      canvas->pencolor = canvas->colorPalette[colIndex];

      int tmphue = canvas->fgHu;
      canvas->RGB2HSB();
      int tmphue2 = canvas->fgHu;
      canvas->fgHu = tmphue;
      canvas->toolbar->reverseSBCursor();
      canvas->huebar->changeHueArea(COLORDIV-tmphue2, -1);
      canvas->toolbar->changeSBArea();
      canvas->toolbar->changeColorArea();

      canvas->toolbar->repaint_left = SBAREA_X-2;
      canvas->toolbar->repaint_top = SBAREA_Y-2;
      canvas->toolbar->repaint_width = SBAREA_WIDTH+4;
      canvas->toolbar->repaint_height = SBAREA_HEIGHT+4;
      QRegion r(canvas->toolbar->repaint_left,canvas->toolbar->repaint_top,canvas->toolbar->repaint_width,canvas->toolbar->repaint_height,QRegion::Rectangle);
      canvas->toolbar->repaint(r, false);

      canvas->toolbar->repaint_left = COLORAREA_X;
      canvas->toolbar->repaint_top = COLORAREA_Y;
      canvas->toolbar->repaint_width = COLORAREA_WIDTH;
      canvas->toolbar->repaint_height = COLORAREA_HEIGHT;
      r =QRegion(canvas->toolbar->repaint_left,canvas->toolbar->repaint_top,canvas->toolbar->repaint_width,canvas->toolbar->repaint_height,QRegion::Rectangle);
      canvas->toolbar->repaint(r, false);

      canvas->toolbar->repaint_left = COLORAREA3_X;
      canvas->toolbar->repaint_top = COLORAREA3_Y;
      canvas->toolbar->repaint_width = COLORAREA3_WIDTH;
      canvas->toolbar->repaint_height = COLORAREA3_HEIGHT;
      r =QRegion(canvas->toolbar->repaint_left,canvas->toolbar->repaint_top,canvas->toolbar->repaint_width,canvas->toolbar->repaint_height,QRegion::Rectangle);
      canvas->toolbar->repaint(r, false);

      int repaint_left;
      int repaint_top;
      int repaint_width;
      int repaint_height;

      repaint_left = 479-(int)(479*canvas->fgHu/COLORDIV)-1;
      if(repaint_left < 0){
	repaint_left = 0;
      }
      else if(repaint_left > 477){
	repaint_left = 477;
      }
      repaint_top = 0;
      repaint_width = 3;
      repaint_height = 20;
      canvas->toolbar->repaintColor(repaint_left, repaint_top, repaint_width, repaint_height, false);

    }

  }
  else{
    if(colIndex != -1){
      canvas->colorPalette[colIndex] = canvas->pencolor;
      paletteSpoit = true;
    }
  }

  //スティッキー
  if(e->y() >= 0 && e->y() < 17){
    if(e->x() >= 32 && e->x() < 46){
      stickyFlag = true;
      this->hide();
      display = false;
    }
    else if(e->x() >= 47 && e->x() < 61){
      stickyFlag = false;
      this->hide();
      display = false;
      this->move(PALETTEPANEL_X,canvas->palettePanelY);
    }
  }

  if(!stickyFlag){
     this->hide();
      display = false;
  }


  myReset();

}
