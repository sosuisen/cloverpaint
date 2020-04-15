#include "clover.h"
#include "lupe_normal.h"
#include "lupe_reverse.h"
#include "lupe_normal2.h"
#include "lupe_reverse2.h"

#define LUPEICON_X 126
#define LUPEICON_Y 31
#define LUPEICON_WIDTH 29
#define LUPEICON_HEIGHT 29


Lupe::Lupe(Canvas * parent, const char * name, bool modal, WFlags f)
  :QDialog(parent, name, modal, f)
{
  canvas = parent;

  moveFlag = false;
  drawFlag = false;
  closePressFlag = false;

  reparent( parent, QPoint(LUPEPANEL_X, LUPEPANEL_Y1));
  resize(227, 246);

  changeWidget();


}

Lupe::~Lupe()
{
}

void Lupe::changeWidget(){
  //画像の読み込み
  QImage tmpImg;
  if(canvas->canvasType == VGA){
      tmpImg= QImage(lupe_normal);
  }
  else if(canvas->canvasType == QVGA){
    tmpImg= QImage(lupe_normal2);
  }
  //xpmからの読みこみはインデックスカラーになるため変換
  lupeImg = tmpImg.convertDepth(32);
  lupe_normalImg = tmpImg.convertDepth(32);

  //画像(裏)の読み込み
  if(canvas->canvasType == VGA){
    tmpImg = QImage(lupe_reverse);
  }
  else if(canvas->canvasType == QVGA){
    tmpImg = QImage(lupe_reverse2);
  }
  lupe_reverseImg = tmpImg.convertDepth(32);


  if(canvas->lupeScale == 2){
    for(int y=2; y<19; y++){
      for(int x=13; x<35; x++){
	lupeImg.setPixel(x,y,lupe_reverseImg.pixel(x,y));
      }
    }
  }
  else if(canvas->lupeScale == 3){
    for(int y=2; y<19; y++){
      for(int x=36; x<57; x++){
	lupeImg.setPixel(x,y,lupe_reverseImg.pixel(x,y));
      }
    }
  }
  else if(canvas->lupeScale == 4){
    for(int y=2; y<19; y++){
      for(int x=59; x<80; x++){
	lupeImg.setPixel(x,y,lupe_reverseImg.pixel(x,y));
      }
    }
  }

  //ツールバー画像の表示
  QPixmap lupePix = QPixmap();
  lupePix.convertFromImage(lupeImg, 0);
  setBackgroundPixmap(lupePix);

}

void Lupe::myReset(){

  int div = 1;
  if(canvas->canvasType == QVGA){
    div = 2;
  }
  for(int y=20; y<244; y++){
    for(int x=1; x<225; x++){
      if((((y-20)>>canvas->lupeScale)+canvas->lupeTop) >= SCREEN_HEIGHT/div
	 || (((y-20)>>canvas->lupeScale)+canvas->lupeTop) < 0
	 || (((x-1)>>canvas->lupeScale)+canvas->lupeLeft)  >= SCREEN_WIDTH/div
	 || (((x-1)>>canvas->lupeScale)+canvas->lupeLeft) < 0){
	lupeImg.setPixel(x,y,0xff808080);
      }
      else{
	if(canvas->canvasType == VGA){
	  if(canvas->useColorMatrix){
	    lupeImg.setPixel(x,y,
			     0xff000000
			     | (( canvas->colorMatrix[0][(canvas->bgImage->pixel(((x-1)>>canvas->lupeScale)+canvas->lupeLeft,((y-20)>>canvas->lupeScale)+canvas->lupeTop)>>16) & 0x000000ff] << 16) & 0x00ff0000 )
			     | (( canvas->colorMatrix[1][(canvas->bgImage->pixel(((x-1)>>canvas->lupeScale)+canvas->lupeLeft,((y-20)>>canvas->lupeScale)+canvas->lupeTop)>>8) & 0x000000ff] << 8) & 0x0000ff00 )
			     | (( canvas->colorMatrix[2][(canvas->bgImage->pixel(((x-1)>>canvas->lupeScale)+canvas->lupeLeft,((y-20)>>canvas->lupeScale)+canvas->lupeTop)) & 0x000000ff] ) & 0x000000ff ));
	    
	  }
	  else{
	    lupeImg.setPixel(x,y,
			     canvas->bgImage->pixel(((x-1)>>canvas->lupeScale)+canvas->lupeLeft,((y-20)>>canvas->lupeScale)+canvas->lupeTop));
	  }
	}
	else if(canvas->canvasType == QVGA){
	  if(canvas->useColorMatrix){
	    lupeImg.setPixel(x,y,
			     0xff000000
			     | (( canvas->colorMatrix[0][(canvas->bgImage->pixel((((x-1)>>canvas->lupeScale)+canvas->lupeLeft)*2,(((y-20)>>canvas->lupeScale)+canvas->lupeTop)*2)>>16) & 0x000000ff] << 16) & 0x00ff0000 )
			     | (( canvas->colorMatrix[1][(canvas->bgImage->pixel((((x-1)>>canvas->lupeScale)+canvas->lupeLeft)*2,(((y-20)>>canvas->lupeScale)+canvas->lupeTop)*2)>>8) & 0x000000ff] << 8) & 0x0000ff00 )
			     | (( canvas->colorMatrix[2][(canvas->bgImage->pixel((((x-1)>>canvas->lupeScale)+canvas->lupeLeft)*2,(((y-20)>>canvas->lupeScale)+canvas->lupeTop)*2)) & 0x000000ff] ) & 0x000000ff ));

	  }
	  else{
	    lupeImg.setPixel(x,y,
			     canvas->bgImage->pixel((((x-1)>>canvas->lupeScale)+canvas->lupeLeft)*2,(((y-20)>>canvas->lupeScale)+canvas->lupeTop)*2));
	  }

	}
      }
    }
  }

  QPixmap lupePix = QPixmap();
  lupePix.convertFromImage(lupeImg, 0);
  setBackgroundPixmap(lupePix);

  //  repaint();

}


void Lupe::showEvent( QShowEvent *e ){
  this->setActiveWindow();
  this->raise();

  for(int y=3; y<17; y++){
    for(int x=209; x<223; x++){
      lupeImg.setPixel(x,y,lupe_normalImg.pixel(x,y));
    }
  }

  for(int y=LUPEICON_Y; y<LUPEICON_Y+LUPEICON_HEIGHT; y++){
    for(int x=LUPEICON_X; x<LUPEICON_X+LUPEICON_WIDTH; x++){
      canvas->toolbar->toolbarImg.setPixel(x,y,canvas->toolbar->toolbar_reverseImg.pixel(x,y));
    }
  }

  QPixmap toolbarPix = QPixmap();
  toolbarPix.convertFromImage(canvas->toolbar->toolbarImg, 0);
  canvas->toolbar->setBackgroundPixmap(toolbarPix);

  int div = 1;
  if(canvas->canvasType == QVGA){
    div = 2;
  }

  for(int y=20; y<244; y++){
    for(int x=1; x<225; x++){
      if((((y-20)>>canvas->lupeScale)+canvas->lupeTop) >= SCREEN_HEIGHT/div 
	 || (((y-20)>>canvas->lupeScale)+canvas->lupeTop) < 0
	 || (((x-1)>>canvas->lupeScale)+canvas->lupeLeft)  >= SCREEN_WIDTH/div
	 || (((x-1)>>canvas->lupeScale)+canvas->lupeLeft) < 0){
	lupeImg.setPixel(x,y,0xff808080);
      }
      else{

	if(canvas->canvasType == VGA){
	if(canvas->useColorMatrix){
	  lupeImg.setPixel(x,y,
			    0xff000000
			    | (( canvas->colorMatrix[0][(canvas->bgImage->pixel(((x-1)>>canvas->lupeScale)+canvas->lupeLeft,((y-20)>>canvas->lupeScale)+canvas->lupeTop)>>16) & 0x000000ff] << 16) & 0x00ff0000 )
			    | (( canvas->colorMatrix[1][(canvas->bgImage->pixel(((x-1)>>canvas->lupeScale)+canvas->lupeLeft,((y-20)>>canvas->lupeScale)+canvas->lupeTop)>>8) & 0x000000ff] << 8) & 0x0000ff00 )
			   | (( canvas->colorMatrix[2][(canvas->bgImage->pixel(((x-1)>>canvas->lupeScale)+canvas->lupeLeft,((y-20)>>canvas->lupeScale)+canvas->lupeTop)) & 0x000000ff] ) & 0x000000ff ));
	}
	else{
	  lupeImg.setPixel(x,y,canvas->bgImage->pixel(((x-1)>>canvas->lupeScale)+canvas->lupeLeft,((y-20)>>canvas->lupeScale)+canvas->lupeTop));
	}
	}
	else if(canvas->canvasType == QVGA){
	if(canvas->useColorMatrix){
	  lupeImg.setPixel(x,y,
			    0xff000000
			    | (( canvas->colorMatrix[0][(canvas->bgImage->pixel((((x-1)>>canvas->lupeScale)+canvas->lupeLeft)*2,(((y-20)>>canvas->lupeScale)+canvas->lupeTop)*2)>>16) & 0x000000ff] << 16) & 0x00ff0000 )
			    | (( canvas->colorMatrix[1][(canvas->bgImage->pixel((((x-1)>>canvas->lupeScale)+canvas->lupeLeft)*2,(((y-20)>>canvas->lupeScale)+canvas->lupeTop)*2)>>8) & 0x000000ff] << 8) & 0x0000ff00 )
			   | (( canvas->colorMatrix[2][(canvas->bgImage->pixel((((x-1)>>canvas->lupeScale)+canvas->lupeLeft)*2,(((y-20)>>canvas->lupeScale)+canvas->lupeTop)*2)) & 0x000000ff] ) & 0x000000ff ));
	}
	else{
	  lupeImg.setPixel(x,y,canvas->bgImage->pixel((((x-1)>>canvas->lupeScale)+canvas->lupeLeft)*2,(((y-20)>>canvas->lupeScale)+canvas->lupeTop)*2));
	}
	}

      }




    }
  }
  QPixmap lupePix = QPixmap();
  lupePix.convertFromImage(lupeImg, 0);
  setBackgroundPixmap(lupePix);


}

void Lupe::hideEvent( QHideEvent *e ){

      for(int y=LUPEICON_Y; y<LUPEICON_Y+LUPEICON_HEIGHT; y++){
	for(int x=LUPEICON_X; x<LUPEICON_X+LUPEICON_WIDTH; x++){
	  canvas->toolbar->toolbarImg.setPixel(x,y,canvas->toolbar->toolbar_normalImg.pixel(x,y));
	}
      }
  canvas->toolbar->redrawToolBar();

      //      QPixmap toolbarPix = QPixmap();
      //      toolbarPix.convertFromImage(canvas->toolbar->toolbarImg, 0);
      //      canvas->toolbar->setBackgroundPixmap(toolbarPix);

}

void Lupe::mousePressEvent( QMouseEvent *e )
{
  closePressFlag = false;

  if(!this->isTopLevel()){
    this->setActiveWindow();
    this->raise();
  }

  if(e->y() >= 0 && e->y() < 20){
    if(e->x() >= 15 && e->x()<31){

      //    canvas->bgImage->save("spoit2.png","PNG");

	if(canvas->lupeScale == 3){
	  canvas->lupeTop -= LUPE_HEIGHT/2/4 - LUPE_HEIGHT/2/8;
	  canvas->lupeLeft -= LUPE_WIDTH/2/4 - LUPE_WIDTH/2/8;
	}
	else if(canvas->lupeScale == 4){
	  canvas->lupeTop -= LUPE_HEIGHT/2/4 - LUPE_HEIGHT/2/16;
	  canvas->lupeLeft -= LUPE_WIDTH/2/4 - LUPE_WIDTH/2/16;
	}
	canvas->lupeScale = 2;

	for(int y=2; y<19; y++){
	  for(int x=13; x<35; x++){
	    lupeImg.setPixel(x,y,lupe_reverseImg.pixel(x,y));
	  }
	}
	for(int y=2; y<19; y++){
	  for(int x=36; x<57; x++){
	    lupeImg.setPixel(x,y,lupe_normalImg.pixel(x,y));
	  }
	}
	for(int y=2; y<19; y++){
	  for(int x=59; x<80; x++){
	    lupeImg.setPixel(x,y,lupe_normalImg.pixel(x,y));
	  }
	}


	QPixmap lupePix = QPixmap();
	lupePix.convertFromImage(lupeImg, 0);
	setBackgroundPixmap(lupePix);
	
	myReset();

      }
      else if(e->x() >= 38 && e->x()<54){
	  if(canvas->lupeScale == 2){
	    canvas->lupeTop += LUPE_HEIGHT/2/4 - LUPE_HEIGHT/2/8;
	    canvas->lupeLeft += LUPE_WIDTH/2/4 - LUPE_WIDTH/2/8;
	  }
	  else if(canvas->lupeScale == 4){
	    canvas->lupeTop -= LUPE_HEIGHT/2/8 - LUPE_HEIGHT/2/16;
	    canvas->lupeLeft -= LUPE_WIDTH/2/8 - LUPE_WIDTH/2/16;
	  }
	  canvas->lupeScale = 3;


	for(int y=2; y<19; y++){
	  for(int x=13; x<35; x++){
	    lupeImg.setPixel(x,y,lupe_normalImg.pixel(x,y));
	  }
	}
	for(int y=2; y<19; y++){
	  for(int x=36; x<57; x++){
	    lupeImg.setPixel(x,y,lupe_reverseImg.pixel(x,y));
	  }
	}
	for(int y=2; y<19; y++){
	  for(int x=59; x<80; x++){
	    lupeImg.setPixel(x,y,lupe_normalImg.pixel(x,y));
	  }
	}

	QPixmap lupePix = QPixmap();
	lupePix.convertFromImage(lupeImg, 0);
	setBackgroundPixmap(lupePix);

	myReset();
      }
      else if(e->x() >= 61 && e->x()<77){
	  if(canvas->lupeScale == 2){
	    canvas->lupeTop += LUPE_HEIGHT/2/4 - LUPE_HEIGHT/2/16;
	    canvas->lupeLeft += LUPE_WIDTH/2/4 - LUPE_WIDTH/2/16;
	  }
	  else if(canvas->lupeScale == 3){
	    canvas->lupeTop += LUPE_HEIGHT/2/8 - LUPE_HEIGHT/2/16;
	    canvas->lupeLeft += LUPE_WIDTH/2/8 - LUPE_WIDTH/2/16;
	  }
	  canvas->lupeScale = 4;


	for(int y=2; y<19; y++){
	  for(int x=13; x<35; x++){
	    lupeImg.setPixel(x,y,lupe_normalImg.pixel(x,y));
	  }
	}
	for(int y=2; y<19; y++){
	  for(int x=36; x<57; x++){
	    lupeImg.setPixel(x,y,lupe_normalImg.pixel(x,y));
	  }
	}
	for(int y=2; y<19; y++){
	  for(int x=59; x<80; x++){
	    lupeImg.setPixel(x,y,lupe_reverseImg.pixel(x,y));
	  }
	}


	QPixmap lupePix = QPixmap();
	lupePix.convertFromImage(lupeImg, 0);
	setBackgroundPixmap(lupePix);

	myReset();

      }
    else if(e->x() >= 209){
      // Closeアイコン
      closePressFlag = true;
      for(int y=3; y<17; y++){
	for(int x=209; x<223; x++){
	  lupeImg.setPixel(x,y,(~(lupe_reverseImg.pixel(x,y))) | 0xff000000);
	}
      }

      QPixmap lupePix = QPixmap();
      lupePix.convertFromImage(lupeImg, 0);
      setBackgroundPixmap(lupePix);

      return;
    }
    else{
      prev_x = e->x();
      prev_y = e->y();

      moveFlag = true;
 
      return;
    }
  }
  else if(e->y() >= 20 && e->y() < 241){
    if(e->x() >= 1 && e->x() < 225){    
      drawFlag = true;
      canvas->prevpoint->setX(canvas->lupeLeft+((e->x()-1)>>canvas->lupeScale));
      canvas->prevpoint->setY(canvas->lupeTop+((e->y()-20)>>canvas->lupeScale));

  if(canvas->bokeashiFlag){
    for(int y=canvas->undo_top; y<=canvas->undo_bottom; y++){
      for(int x=canvas->undo_left; x<=canvas->undo_right; x++){
	*(canvas->redoBuffer+x+y*480) = 0x00ffffff;
      }
    }
  }


      if(canvas->undo_top != 640 && canvas->undo_left != 480){
	for(int y=canvas->undo_top; y<=canvas->undo_bottom; y++){
	  for(int x=canvas->undo_left; x<=canvas->undo_right; x++){
	    *(canvas->undoBuffer+x+y*480) = canvas->layer[canvas->currentlayer]->pixel(x,y);
	  }
	}
	canvas->undo_left = 480;
	canvas->undo_top = 640;
	canvas->undo_right = 480;
	canvas->undo_bottom = 640;
      }


      canvas->penDownFlag = true;
      canvas->penPressFlag = true;

      return;
    }
  }


  if(canvas->spoitSelectFlag){
    if(canvas->prevpoint->x()<0 || canvas->prevpoint->x()>=480 || canvas->prevpoint->y() < 0 || canvas->prevpoint->y() >= 640)
      return;

    canvas->toolbar->reverseSBCursor();
    
    if(canvas->canvasType == VGA){
      canvas->pencolor = canvas->bgImage->pixel(canvas->prevpoint->x(),canvas->prevpoint->y());
    }
    else if(canvas->canvasType == QVGA){
      canvas->pencolor = canvas->bgImage->pixel(canvas->prevpoint->x()*2,canvas->prevpoint->y()*2);
    }
    int tmphue = canvas->fgHu;
    canvas->RGB2HSB();
    int tmphue2 = canvas->fgHu;
    canvas->fgHu = tmphue;
    canvas->toolbar->reverseSBCursor();
    canvas->huebar->changeHueArea(COLORDIV-tmphue2, -1);
    canvas->toolbar->changeSBArea();
    canvas->toolbar->changeColorArea();

    //ツールバー画像の表示
    //    QPixmap toolbarPix = QPixmap();
    //    toolbarPix.convertFromImage(canvas->toolbar->toolbarImg, 0);
    //    canvas->toolbar->setBackgroundPixmap(toolbarPix);
    
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

    //    QPixmap huePix = QPixmap();
    //    huePix.convertFromImage(canvas->huebar->hueImg, 0);
    //    canvas->huebar->setBackgroundPixmap(huePix);
    
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



void Lupe::mouseMoveEvent( QMouseEvent *e )
{
  int next_x, next_y;
  closePressFlag = false;

  if(moveFlag){
    next_x = x()+e->x()-prev_x;
    next_y = y()+e->y()-prev_y;
  
    if(next_x < 0){
      next_x = 0;
    }
    else if(next_x > 480-227){
      next_x = 480-227;
    }

    if(next_y < 0){
      next_y = 0;
    }
    else if(next_y > 640-246){
      next_y = 640-246;
    }
    move(next_x, next_y);
    
    myReset();
    return;
  }

  canvas->point->setX(canvas->lupeLeft+((e->x()-1)>>canvas->lupeScale));
  canvas->point->setY(canvas->lupeTop+((e->y()-20)>>canvas->lupeScale));


  if(canvas->spoitSelectFlag){
    if(canvas->point->x()<0 || canvas->point->x()>=480 || canvas->point->y() < 0 || canvas->point->y() >= 640)
      return;

    canvas->toolbar->reverseSBCursor();

    if(canvas->canvasType == VGA){
      canvas->pencolor = canvas->bgImage->pixel(canvas->point->x(),canvas->point->y());
    }
    else if(canvas->canvasType == QVGA){
      canvas->pencolor = canvas->bgImage->pixel(canvas->point->x()*2,canvas->point->y()*2);
    }

    int tmphue = canvas->fgHu;
    canvas->RGB2HSB();
    int tmphue2 = canvas->fgHu;
    canvas->fgHu = tmphue;
    canvas->toolbar->reverseSBCursor();
    canvas->huebar->changeHueArea(COLORDIV-tmphue2, -1);
    canvas->toolbar->changeSBArea();
    canvas->toolbar->changeColorArea();

    //ツールバー画像の表示
    //QPixmap toolbarPix = QPixmap();
    //toolbarPix.convertFromImage(canvas->toolbar->toolbarImg, 0);
    //canvas->toolbar->setBackgroundPixmap(toolbarPix);
    
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

    //    QPixmap huePix = QPixmap();
    //    huePix.convertFromImage(canvas->huebar->hueImg, 0);
    //    canvas->huebar->setBackgroundPixmap(huePix);

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


  if(drawFlag){
    if(canvas->fillFlag){
      //nop
      canvas->prevpoint->setX(canvas->point->x());
      canvas->prevpoint->setY(canvas->point->y());

      return;
    }
    else{
      canvas->fileChangeFlag = true;
      if(canvas->penDownFlag){
	canvas->drawLine(true, false);
	canvas->penDownFlag = false;
      }
      else{
	canvas->drawLine(false, false);
      }
      canvas->repaintRect();

      canvas->prevpoint->setX(canvas->point->x());
      canvas->prevpoint->setY(canvas->point->y());

      return;
    }
  }

  if(e->y() >= 0 && e->y() < 20 && e->x() >= 209){
    // Closeアイコン
    closePressFlag = true;
  }

  if(closePressFlag){
    for(int y=3; y<17; y++){
      for(int x=209; x<223; x++){
	lupeImg.setPixel(x,y,(~(lupe_reverseImg.pixel(x,y))) | 0xff000000);
      }
    }
  }
  else{
    for(int y=3; y<17; y++){
      for(int x=209; x<223; x++){
	lupeImg.setPixel(x,y,lupe_normalImg.pixel(x,y));
      }
    }
  }
  QPixmap lupePix = QPixmap();
  lupePix.convertFromImage(lupeImg, 0);
  setBackgroundPixmap(lupePix);

}



void Lupe::mouseReleaseEvent( QMouseEvent *e ){

  int next_x, next_y;
  closePressFlag = false;

  if(moveFlag){
    next_x = x()+e->x()-prev_x;
    next_y = y()+e->y()-prev_y;
  
    if(next_x < 0){
      next_x = 0;
    }
    else if(next_x > 480-227){
      next_x = 480-227;
    }

    if(next_y < 0){
      next_y = 0;
    }
    else if(next_y > 640-246){
      next_y = 640-246;
    }

    move(next_x, next_y);
    myReset();
    moveFlag = false;
    canvas->toolbar->redrawToolBar();
    return;
  }


  if(e->x() >= 209){
    if(e->y() < 20){
      if(canvas->canvasType == VGA){
	canvas->zoomStyle = NORMAL;
      }
      else if(canvas->canvasType == QVGA){
	canvas->zoomStyle = ZOOM;
      }
      

      this->hide();
      myReset();
      return;
    }
  }

  canvas->point->setX(canvas->lupeLeft+((e->x()-1)>>canvas->lupeScale));
  canvas->point->setY(canvas->lupeTop+((e->y()-20)>>canvas->lupeScale));

  if(canvas->spoitSelectFlag){
    if(canvas->point->x()<0 || canvas->point->x()>=480 || canvas->point->y() < 0 || canvas->point->y() >= 640)
      return;

    canvas->toolbar->reverseSBCursor();

    if(canvas->canvasType == VGA){
      canvas->pencolor = canvas->bgImage->pixel(canvas->point->x(),canvas->point->y());
    }
    else if(canvas->canvasType == QVGA){
      canvas->pencolor = canvas->bgImage->pixel(canvas->point->x()*2,canvas->point->y()*2);
    }



    int tmphue = canvas->fgHu;
    canvas->RGB2HSB();
    int tmphue2 = canvas->fgHu;
    canvas->fgHu = tmphue;
    canvas->toolbar->reverseSBCursor();
    canvas->huebar->changeHueArea(COLORDIV-tmphue2, -1);
    canvas->toolbar->changeSBArea();
    canvas->toolbar->changeColorArea();

    //ツールバー画像の表示
    //    canvas->spoitSelectFlag = false;
    //    canvas->toolbar->redrawToolBar();

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

    
    //    QPixmap toolbarPix = QPixmap();
    //    toolbarPix.convertFromImage(canvas->toolbar->toolbarImg, 0);
    //    canvas->toolbar->setBackgroundPixmap(toolbarPix);

    //    QPixmap huePix = QPixmap();
    //    huePix.convertFromImage(canvas->huebar->hueImg, 0);
    //    canvas->huebar->setBackgroundPixmap(huePix);
    
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

    //    canvas->toolbar->repaint();

    return;
  }


  if(drawFlag){
    drawFlag = false;

    if(canvas->fillFlag){
    int div = 1;
    if(canvas->canvasType == QVGA)
      div = 2;
    if(canvas->point->x() >= 0 && canvas->point->x() < SCREEN_WIDTH/div 
       && canvas->point->y() >= 0 && canvas->point->y() < SCREEN_HEIGHT/div ){

      canvas->fill(canvas->point->x(),canvas->point->y());
      canvas->repaintRectFlag = true;
      canvas->repaint_left = 0;
      canvas->repaint_top = 0;;
      canvas->repaint_width = SCREEN_WIDTH;
      canvas->repaint_height = SCREEN_HEIGHT;

      QRegion r;
      r = QRegion(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,QRegion::Rectangle);

      canvas->repaint(r, false);

      if(canvas->undoFlag){
	canvas->undoFlag = false;
	canvas->toolbar->redrawToolBar();
      }

      myReset();

      canvas->undo_left = 0;
      canvas->undo_top = 0;
      canvas->undo_right = 479;
      canvas->undo_bottom = 639;
    }
    }
    else{

      canvas->drawLine(false, true);
      canvas->repaintRect();


      canvas->penPressFlag = false;

      if(canvas->bokeashiFlag){
	for(int y=canvas->undo_top; y<=canvas->undo_bottom; y++){
	  for(int x=canvas->undo_left; x<=canvas->undo_right; x++){
	    if(((*(canvas->drawChecker+y*SCREEN_WIDTH+x)>>24) & 0x000000ff) > 0){
	      canvas->drawPoint(x,y,*(canvas->drawChecker+y*SCREEN_WIDTH+x), -1, false);
	    }
	  }
	}
      }

      myReset();

      if(canvas->undoFlag){
	canvas->undoFlag = false;
	canvas->toolbar->redrawToolBar();
      }
    }    

    for(int i=0; i<SCREEN_WIDTH*SCREEN_HEIGHT; i++){
      *(canvas->drawChecker+i) = 0x00ffffff;
    }

  }



}
