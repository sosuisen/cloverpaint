#include "clover.h"
#include "scale_normal.h"
#include "scale_reverse.h"
#include "select_center.h"

Scale::Scale(Canvas * parent, const char * name, bool modal, WFlags f)
  :QDialog(parent, name, modal, f)
{
  canvas = parent;

  moveFlag = false;
  stickyFlag = false;
  minimizePressFlag = false;
  closePressFlag = false;
  display = false;

  reparent( parent, QPoint(SCALEPANEL_X, canvas->scalePanelY));
  resize(65, 111);

  //画像の読み込み
  QImage tmpImg = QImage(scale_normal);
  //xpmからの読みこみはインデックスカラーになるため変換
  scaleImg = tmpImg.convertDepth(32);
  scale_normalImg = tmpImg.convertDepth(32);

  tmpImg = QImage(scale_reverse);
  scale_reverseImg = tmpImg.convertDepth(32);

  tmpImg = QImage(select_center);
  select_centerImg = tmpImg.convertDepth(32);


  //画像の表示
  QPixmap scalePix = QPixmap();
  scalePix.convertFromImage(scaleImg, 0);
  setBackgroundPixmap(scalePix);

}

Scale::~Scale()
{
}


void Scale::myReset(){
    if(canvas->canvasType == VGA){
      if(canvas->zoomStyle != ZOOM){
	changeScale(0, 0);
      }
      else{
	changeScale(canvas->zoomScale, 0);
      }
    }
    else if(canvas->canvasType == QVGA){
      changeScale(canvas->zoomScale-1, 0);
    }

}


void Scale::showEvent( QShowEvent *e ){
  this->setActiveWindow();
  this->raise();


  if(!stickyFlag && (this->y() != canvas->scalePanelY)){
    move(SCALEPANEL_X, canvas->scalePanelY);
  }
  myReset();
}

void Scale::hideEvent( QHideEvent *e ){
    for(int y=SCALEICON_Y; y<SCALEICON_Y+SCALEICON_HEIGHT; y++){
      for(int x=SCALEICON_X; x<SCALEICON_X+SCALEICON_WIDTH; x++){
        canvas->toolbar->toolbarImg.setPixel(x,y,canvas->toolbar->toolbar_normalImg.pixel(x,y));
      }
    }
    QPixmap toolbarPix = QPixmap();
    toolbarPix.convertFromImage(canvas->toolbar->toolbarImg, 0);
    canvas->toolbar->setBackgroundPixmap(toolbarPix);

}

void Scale::paintEvent( QPaintEvent *e ){
  // 他のダイアログとデッドロックを起こすので以下は利用しないこと
  /*

  */

}

void Scale::changeScale(int scale, int select_center){

  int left=1;
  int top=1;

  switch(scale){
  case 0:
   left = 32;
   top = 20;
   break;
  case 1:
    left = 32;
    top = 49;
    break;
  case 2:
    left = 32;
    top = 78;	  
    break;
    //  case 5:
    //    left = 3;
    //    top = 20;	  
    //    break;
  case 3:
    left = 3;
    top = 49;
    break;
  case 4:
    left = 3;
    top = 78;	  
    break;
  default:
    break;
  }
  
  scaleImg = scale_normalImg.copy();

  for(int y=top; y<top+29; y++){
    for(int x=left; x<left+29; x++){
      scaleImg.setPixel(x,y,scale_reverseImg.pixel(x,y));
    }
  }

  if(stickyFlag){
    for(int y=3; y<17; y++){
      for(int x=32; x<61; x++){
	scaleImg.setPixel(x,y,scale_reverseImg.pixel(x,y));
      }
    }
  }
  else{
    for(int y=3; y<17; y++){
      for(int x=32; x<61; x++){
	scaleImg.setPixel(x,y,scale_normalImg.pixel(x,y));
      }
    }
  }


    if(minimizePressFlag){
      for(int y=3; y<17; y++){
	for(int x=32; x<46; x++){
	  scaleImg.setPixel(x,y,(~(scale_reverseImg.pixel(x,y))) | 0xff000000);
	}
      }
    }
    else{
      if(stickyFlag){
	for(int y=3; y<17; y++){
	  for(int x=32; x<46; x++){
	    scaleImg.setPixel(x,y,((scale_reverseImg.pixel(x,y))) | 0xff000000);
	  }
	}
      }
      else{
	for(int y=3; y<17; y++){
	  for(int x=32; x<46; x++){
	    scaleImg.setPixel(x,y,((scale_normalImg.pixel(x,y))) | 0xff000000);
	  }
	}
      }

    }

    if(closePressFlag){
      for(int y=3; y<17; y++){
	for(int x=47; x<61; x++){
	  scaleImg.setPixel(x,y,(~(scale_reverseImg.pixel(x,y))) | 0xff000000);
	}
      }
    }
    else{
      if(stickyFlag){
	for(int y=3; y<17; y++){
	  for(int x=47; x<61; x++){
	    scaleImg.setPixel(x,y,((scale_reverseImg.pixel(x,y))) | 0xff000000);
	  }
	}
      }
      else{
	for(int y=3; y<17; y++){
	  for(int x=47; x<61; x++){
	    scaleImg.setPixel(x,y,((scale_normalImg.pixel(x,y))) | 0xff000000);
	  }
	}
      }


    }


  if(canvas->zoomStyle == LUPE){
    for(int y=0; y<25; y++){
      for(int x=0; x<25; x++){
	canvas->scalePanel->scaleImg.setPixel(x+5,y+22,canvas->scalePanel->select_centerImg.pixel(x,y));
      }
    }
  }
  else if(select_center == 1 || canvas->zoomSelectFlag){
    for(int y=20; y<20+29; y++){
      for(int x=3; x<3+29; x++){
	canvas->scalePanel->scaleImg.setPixel(x,y,canvas->scalePanel->scale_reverseImg.pixel(x,y));
      }
    }
  }
  else if(scale == 0){
    for(int y=20; y<20+29; y++){
      for(int x=3; x<3+29; x++){
	canvas->scalePanel->scaleImg.setPixel(x,y,canvas->scalePanel->scale_normalImg.pixel(x,y));
      }
    }
  }
  else{
    for(int y=0; y<25; y++){
      for(int x=0; x<25; x++){
	canvas->scalePanel->scaleImg.setPixel(x+5,y+22,canvas->scalePanel->select_centerImg.pixel(x,y));
      }
    }

  }

  QPixmap scalePix = QPixmap();
  scalePix.convertFromImage(scaleImg, 0);
  setBackgroundPixmap(scalePix);


  if(scale == 0){
    return;
  }

}



void Scale::mousePressEvent( QMouseEvent *e )
{
  int top=1, left=1;
  minimizePressFlag = false;
  closePressFlag = false;

  if(!this->isTopLevel()){
    this->setActiveWindow();
    this->raise();
  }

  if(e->y() >= 0 && e->y() < 17){
    if(e->x() >= 0 && e->x() < 32){

      canvas->zoomSelectFlag = false;

      prev_x = e->x();
      prev_y = e->y();

      moveFlag = true;
      stickyFlag = true;

      changeScale(canvas->zoomScale, 0);

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


    if(e->x() >= 3 && e->x() < 32){
      // 左列
      if(e->y() >= 20 && e->y() < 49){
	if(canvas->canvasType == VGA){
	  if(canvas->zoomScale == 0 && canvas->zoomStyle != LUPE){
	    changeScale(canvas->zoomScale, 1);
	  }
	  else{
	    changeScale(canvas->zoomScale, 0);
	  }
	}
	else if(canvas->canvasType == QVGA){
	  if(canvas->zoomScale == 1 && canvas->zoomStyle != LUPE){
	    changeScale(canvas->zoomScale, 1);
	  }
	  else{
	    changeScale(canvas->zoomScale, 0);
	  }
	}
      }
      else if(e->y() >= 49 && e->y() < 78){
      	changeScale(3, 0);
      }
      else if(e->y() >= 78 && e->y()< 107){
      	changeScale(4, 0);
      }
    }
    else if (e->x()>= 32 && e->x()< 61){
      // 右列
      if(e->y() >= 20 && e->y() < 49){
	changeScale(0, 0);
      }
      else if(e->y() >= 49 && e->y() < 78){
	changeScale(1, 0);
      }
      else if(e->y() >= 78 && e->y()< 107){
	changeScale(2, 0);
      }    
    }



}


void Scale::mouseMoveEvent( QMouseEvent *e )
{
  int top=1, left=1;
  int next_x, next_y;
  minimizePressFlag = false;
  closePressFlag = false;

  if(moveFlag){
    canvas->zoomSelectFlag = false;

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



  if(e->x() >= 3 && e->x() < 32){
    // 左列
    if(e->y() >= 20 && e->y() < 49){
      if(canvas->canvasType == VGA){
	if(canvas->zoomScale == 0 && canvas->zoomStyle != LUPE){
	  changeScale(canvas->zoomScale, 1);
	}
	else{
	  changeScale(canvas->zoomScale, 0);
	}
      }
      else if(canvas->canvasType == QVGA){
	if(canvas->zoomScale == 1 && canvas->zoomStyle != LUPE){
	  changeScale(canvas->zoomScale, 1);
	}
	else{
	  changeScale(canvas->zoomScale, 0);
	}
      }
    }
    else if(e->y() >= 49 && e->y() < 78){
      	changeScale(3, 0);
    }
    else if(e->y() >= 78 && e->y()< 107){
      	changeScale(4, 0);
    }
  }
  else if (e->x()>= 32 && e->x()< 61){
    // 右列
    if(e->y() >= 20 && e->y() < 49){
      changeScale(0, 0);
    }
    else if(e->y() >= 49 && e->y() < 78){
      changeScale(1, 0);
    }
    else if(e->y() >= 78 && e->y()< 107){
      changeScale(2, 0);
    }    
  }

}


void Scale::changeOffset(int scale){

  if(canvas->canvasType == VGA){
    if(canvas->zoomScale == 0){
      canvas->zoomScale = canvas->lastScale;
    }
  }

  if(canvas->zoomScale == 1){
    if(scale == 2){
      canvas->zoomTop += (SCREEN_HEIGHT/2/2 - SCREEN_HEIGHT/2/4);
      canvas->zoomLeft += (SCREEN_WIDTH/2/2 - SCREEN_WIDTH/2/4);
    }
    else if(scale == 3){
      canvas->zoomTop += SCREEN_HEIGHT/2/2 - SCREEN_HEIGHT/2/8;
      canvas->zoomLeft += SCREEN_WIDTH/2/2 - SCREEN_WIDTH/2/8;
    }
    else if(scale == 4){
      canvas->zoomTop += SCREEN_HEIGHT/2/2 - SCREEN_HEIGHT/2/16;
      canvas->zoomLeft += SCREEN_WIDTH/2/2 - SCREEN_WIDTH/2/16;
    }
    else if(scale == 5){
      canvas->zoomTop += SCREEN_HEIGHT/2/2 - SCREEN_HEIGHT/2/32;
      canvas->zoomLeft += SCREEN_WIDTH/2/2 - SCREEN_WIDTH/2/32;
    }

  }
  else if(canvas->zoomScale == 2){
    if(scale == 1){
      canvas->zoomTop -= (SCREEN_HEIGHT/2/2 - SCREEN_HEIGHT/2/4);
      canvas->zoomLeft -= (SCREEN_WIDTH/2/2 - SCREEN_WIDTH/2/4);
    }
    else if(scale == 3){
      canvas->zoomTop += SCREEN_HEIGHT/2/4 - SCREEN_HEIGHT/2/8;
      canvas->zoomLeft += SCREEN_WIDTH/2/4 - SCREEN_WIDTH/2/8;
    }
    else if(scale == 4){
      canvas->zoomTop += SCREEN_HEIGHT/2/4 - SCREEN_HEIGHT/2/16;
      canvas->zoomLeft += SCREEN_WIDTH/2/4 - SCREEN_WIDTH/2/16;
    }
    else if(scale == 5){
      canvas->zoomTop += SCREEN_HEIGHT/2/4 - SCREEN_HEIGHT/2/32;
      canvas->zoomLeft += SCREEN_WIDTH/2/4 - SCREEN_WIDTH/2/32;
    }

  }
  else if(canvas->zoomScale == 3){
    if(scale == 1){
      canvas->zoomTop -= SCREEN_HEIGHT/2/2 - SCREEN_HEIGHT/2/8;
      canvas->zoomLeft -= SCREEN_WIDTH/2/2 - SCREEN_WIDTH/2/8;
    }
    else if(scale == 2){
      canvas->zoomTop -= SCREEN_HEIGHT/2/4 - SCREEN_HEIGHT/2/8;
      canvas->zoomLeft -= SCREEN_WIDTH/2/4 - SCREEN_WIDTH/2/8;
    }
    else if(scale == 4){
      canvas->zoomTop += SCREEN_HEIGHT/2/8 - SCREEN_HEIGHT/2/16;
      canvas->zoomLeft += SCREEN_WIDTH/2/8 - SCREEN_WIDTH/2/16;
    }
    else if(scale == 5){
      canvas->zoomTop += SCREEN_HEIGHT/2/8 - SCREEN_HEIGHT/2/32;
      canvas->zoomLeft += SCREEN_WIDTH/2/8 - SCREEN_WIDTH/2/32;
    }

  }
  else if(canvas->zoomScale == 4){

    if(scale == 1){
      canvas->zoomTop -= SCREEN_HEIGHT/2/2 - SCREEN_HEIGHT/2/16;
      canvas->zoomLeft -= SCREEN_WIDTH/2/2 - SCREEN_WIDTH/2/16;
    }
    else if(scale == 2){
      canvas->zoomTop -= SCREEN_HEIGHT/2/4 - SCREEN_HEIGHT/2/16;
      canvas->zoomLeft -= SCREEN_WIDTH/2/4 - SCREEN_WIDTH/2/16;
    }
    else if(scale == 3){
      canvas->zoomTop -= SCREEN_HEIGHT/2/8 - SCREEN_HEIGHT/2/16;
      canvas->zoomLeft -= SCREEN_WIDTH/2/8 - SCREEN_WIDTH/2/16;
    }
    else if(scale == 5){
      canvas->zoomTop += SCREEN_HEIGHT/2/16 - SCREEN_HEIGHT/2/32;
      canvas->zoomLeft += SCREEN_WIDTH/2/16 - SCREEN_WIDTH/2/32;
    }
  }
  else if(canvas->zoomScale == 5){

    if(scale == 1){
      canvas->zoomTop -= SCREEN_HEIGHT/2/2 - SCREEN_HEIGHT/2/32;
      canvas->zoomLeft -= SCREEN_WIDTH/2/2 - SCREEN_WIDTH/2/32;
    }
    else if(scale == 2){
      canvas->zoomTop -= SCREEN_HEIGHT/2/4 - SCREEN_HEIGHT/2/32;
      canvas->zoomLeft -= SCREEN_WIDTH/2/4 - SCREEN_WIDTH/2/32;
    }
    else if(scale == 3){
      canvas->zoomTop -= SCREEN_HEIGHT/2/8 - SCREEN_HEIGHT/2/32;
      canvas->zoomLeft -= SCREEN_WIDTH/2/8 - SCREEN_WIDTH/2/16;
    }
    else if(scale == 4){
      canvas->zoomTop -= SCREEN_HEIGHT/2/16 - SCREEN_HEIGHT/2/32;
      canvas->zoomLeft -= SCREEN_WIDTH/2/16 - SCREEN_WIDTH/2/32;
    }

  }

  if(canvas->zoomLeft < -(SCREEN_WIDTH>>(scale+1)))
    canvas->zoomLeft = -(SCREEN_WIDTH>>(scale+1));
  if(canvas->zoomLeft > SCREEN_WIDTH - (SCREEN_WIDTH>>(scale+1)))
    canvas->zoomLeft = SCREEN_WIDTH - (SCREEN_WIDTH>>(scale+1));

  if(canvas->zoomTop < -(SCREEN_HEIGHT>>(scale+1)))
    canvas->zoomTop = -(SCREEN_HEIGHT>>(scale+1));
  if(canvas->zoomTop > SCREEN_HEIGHT - (SCREEN_HEIGHT>>(scale+1)))
    canvas->zoomTop = SCREEN_HEIGHT - (SCREEN_HEIGHT>>(scale+1));
}


void Scale::mouseReleaseEvent( QMouseEvent *e ){

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
    canvas->toolbar->redrawToolBar();
    return;
  }



  if(e->x() >= 3 && e->x() < 32){
    // 左列
    if(e->y() >= 20 && e->y() < 49){
      // 拡大位置指定
      if(canvas->canvasType == VGA){
	if(canvas->zoomScale == 0 && canvas->zoomStyle != LUPE){
	  canvas->zoomSelectFlag = !canvas->zoomSelectFlag;
	}
      }
      else{
	if(canvas->zoomScale == 1 && canvas->zoomStyle != LUPE){
	  canvas->zoomSelectFlag = !canvas->zoomSelectFlag;
	}
      }
      changeScale(canvas->zoomScale, 1);
      canvas->toolbar->redrawToolBar();
    }
    else if(e->y() >= 49 && e->y() < 78){
      if(canvas->canvasType == VGA){
	changeScale(3, 0);
	changeOffset(3);
	canvas->zoomScale = 3;
      }
      else if(canvas->canvasType == QVGA){
	changeScale(3, 0);
	changeOffset(4);
	canvas->zoomScale = 4;
      }

      if(canvas->zoomSelectFlag){
	canvas->zoomSelectFlag = false;
	//	canvas->toolbar->show();
      }

      if(canvas->zoomStyle == LUPE){
	canvas->zoomStyle = NORMAL;
	canvas->lupePanel->hide();

      }

      canvas->zoomStyle = ZOOM;

      canvas->repaint();
    }
    else if(e->y() >= 78 && e->y()< 107){
      if(canvas->canvasType == VGA){
	changeScale(4, 0);
	changeOffset(4);
	canvas->zoomScale = 4;
      }
      else if(canvas->canvasType == QVGA){
	changeScale(4, 0);
	changeOffset(5);
	canvas->zoomScale = 5;
      }


      if(canvas->zoomSelectFlag){
	canvas->zoomSelectFlag = false;
	//	canvas->toolbar->show();
      }

      if(canvas->zoomStyle == LUPE){
	canvas->zoomStyle = NORMAL;
	canvas->lupePanel->hide();
      }


      canvas->zoomStyle = ZOOM;


      canvas->repaint();

    }
  }
  else if (e->x()>= 32 && e->x()< 61){
    // 右列
    if(e->y() >= 20 && e->y() < 49){

      if(canvas->canvasType == VGA){
	if(canvas->zoomScale == 0){
	  canvas->canvasLeft = 0;
	  canvas->canvasTop = 0;
	}
	canvas->zoomSelectFlag = false;
	changeScale(0, 0);
	canvas->zoomStyle = NORMAL;

	if(canvas->canvasType == VGA && canvas->zoomScale > 0)
	  canvas->lastScale = canvas->zoomScale;
	else if(canvas->canvasType == QVGA && canvas->zoomScale > 1)
	  canvas->lastScale = canvas->zoomScale;

      }
      else if(canvas->canvasType == QVGA){
	if(canvas->zoomScale == 1){
	  //	  canvas->canvasLeft = 0;
	  //	  canvas->canvasTop = 0;
	  canvas->zoomLeft = 0;
	  canvas->zoomTop = 0;
	}
	else{
	  changeOffset(1);
	}

	canvas->zoomSelectFlag = false;
	changeScale(0, 0);

	if(canvas->canvasType == VGA && canvas->zoomScale > 0)
	  canvas->lastScale = canvas->zoomScale;
	else if(canvas->canvasType == QVGA && canvas->zoomScale > 1)
	  canvas->lastScale = canvas->zoomScale;

      }

      if(canvas->canvasType == VGA){
	canvas->zoomScale = 0;
      }
      else if(canvas->canvasType == QVGA){
	canvas->zoomScale = 1;
      }

      canvas->repaint();

    }
    else if(e->y() >= 49 && e->y() < 78){

      if(canvas->canvasType == VGA){
	changeScale(1, 0);
	changeOffset(1);
	canvas->zoomScale = 1;
      }
      else if(canvas->canvasType == QVGA){
	changeScale(1, 0);
	changeOffset(2);
	canvas->zoomScale = 2;
      }

      if(canvas->zoomSelectFlag){
	canvas->zoomSelectFlag = false;
	//	canvas->toolbar->show();
      }

      if(canvas->zoomStyle == LUPE){
	canvas->zoomStyle = NORMAL;
	canvas->lupePanel->hide();
      }

      canvas->zoomStyle = ZOOM;


      canvas->repaint();

    }
    else if(e->y() >= 78 && e->y()< 107){

      if(canvas->canvasType == VGA){
	changeScale(2, 0);
	changeOffset(2);
	canvas->zoomScale = 2;
      }
      else if(canvas->canvasType == QVGA){
	changeScale(2, 0);
	changeOffset(3);
	canvas->zoomScale = 3;
      }

      if(canvas->zoomSelectFlag){
	canvas->zoomSelectFlag = false;
	//	canvas->toolbar->show();
      }

      if(canvas->zoomStyle == LUPE){
	canvas->zoomStyle = NORMAL;
	canvas->lupePanel->hide();
      }

      canvas->zoomStyle = ZOOM;


      canvas->repaint();
    }    
  }
  else{
    myReset();
    return;
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
      this->move(SCALEPANEL_X,canvas->scalePanelY);
    }
  }

  if(!stickyFlag){
     this->hide();
      display = false;
  }

  myReset();

  canvas->toolbar->redrawToolBar();


}
