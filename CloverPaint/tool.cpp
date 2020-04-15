#include "clover.h"
#include "tool_normal.h"
#include "tool_reverse.h"


Tool::Tool(Canvas * parent, const char * name, bool modal, WFlags f)
  :QDialog(parent, name, modal, f)
{
  canvas = parent;

  moveFlag = false;
  stickyFlag = false;
  minimizePressFlag = false;
  closePressFlag = false;
  display = false;

  reparent( parent, QPoint(TOOLPANEL_X, canvas->toolPanelY));
  resize(65, 140);

  //画像の読み込み
  QImage tmpImg = QImage(tool_normal);
  //xpmからの読みこみはインデックスカラーになるため変換
  toolImg = tmpImg.convertDepth(32);
  tool_normalImg = tmpImg.convertDepth(32);

  tmpImg = QImage(tool_reverse);
  tool_reverseImg = tmpImg.convertDepth(32);


  //画像の表示
  QPixmap toolPix = QPixmap();
  toolPix.convertFromImage(toolImg, 0);
  setBackgroundPixmap(toolPix);

}

Tool::~Tool()
{
}


void Tool::myReset(){

  for(int y=0; y<this->height(); y++){
    for(int x=0; x<this->width(); x++){
      toolImg.setPixel(x,y,tool_normalImg.pixel(x,y));
    }
  }

  if(stickyFlag){
    for(int y=3; y<17; y++){
      for(int x=32; x<61; x++){
	toolImg.setPixel(x,y,tool_reverseImg.pixel(x,y));
      }
    }
  }
  else{
    for(int y=3; y<17; y++){
      for(int x=32; x<61; x++){
	toolImg.setPixel(x,y,tool_normalImg.pixel(x,y));
      }
    }
  }

    if(minimizePressFlag){
      for(int y=3; y<17; y++){
	for(int x=32; x<46; x++){
	 toolImg.setPixel(x,y,(~(tool_reverseImg.pixel(x,y))) | 0xff000000);
	}
      }
    }
    else{
      if(stickyFlag){
	for(int y=3; y<17; y++){
	  for(int x=32; x<46; x++){
	    toolImg.setPixel(x,y,((tool_reverseImg.pixel(x,y))) | 0xff000000);
	  }
	}
      }
      else{
	for(int y=3; y<17; y++){
	  for(int x=32; x<46; x++){
	    toolImg.setPixel(x,y,((tool_normalImg.pixel(x,y))) | 0xff000000);
	  }
	}
      }

    }

    if(closePressFlag){
      for(int y=3; y<17; y++){
	for(int x=47; x<61; x++){
	  toolImg.setPixel(x,y,(~(tool_reverseImg.pixel(x,y))) | 0xff000000);
	}
      }
    }
    else{
      if(stickyFlag){
	for(int y=3; y<17; y++){
	  for(int x=47; x<61; x++){
	    toolImg.setPixel(x,y,((tool_reverseImg.pixel(x,y))) | 0xff000000);
	  }
	}
      }
      else{
	for(int y=3; y<17; y++){
	  for(int x=47; x<61; x++){
	    toolImg.setPixel(x,y,((tool_normalImg.pixel(x,y))) | 0xff000000);
	  }
	}
      }

    }


  int left=1;
  int top=1;

    if(canvas->fillFlag){
      left = 3;
      top = 107;	  
    }
    else if(canvas->tool == PEN && !canvas->bokeashiFlag){
      left = 3;
      top = 20;	  
    }
    else if(canvas->tool == ERASER && !canvas->bokeashiFlag){
      left = 32;
      top = 20;
    }
    else if(canvas->tool == ERASER && canvas->bokeashiFlag){
      left = 32;
      top = 49;
    }
    else if(canvas->tool == PEN && canvas->bokeashiFlag && !canvas->kasanenuriFlag){
      left = 3;
      top = 49;	  
    }
    else if(canvas->tool == PEN && canvas->bokeashiFlag && canvas->kasanenuriFlag){
      left = 3;
      top = 78;	  
    }
    else if(canvas->tool == BLUR && !canvas->powerBlurFlag){
      left = 32;
      top = 78;
    }
    else if(canvas->tool == BLUR && canvas->powerBlurFlag){
      left = 32;
      top = 107;
    }

  for(int y=top; y<top+29; y++){
    for(int x=left; x<left+29; x++){
      toolImg.setPixel(x,y,tool_reverseImg.pixel(x,y));
    }
  }



  QPixmap toolPix = QPixmap();
  toolPix.convertFromImage(toolImg, 0);
  setBackgroundPixmap(toolPix);



}

void Tool::changeTool(int button_x, int button_y){

  int left=1;
  int top=1;

  if(button_x == 0){
    //左列
    if(button_y == 0){
      left = 3;
      top = 20;	  
    }
    else if(button_y == 1){
      left = 3;
      top = 49;
    }
    else if(button_y == 2){
      left = 3;
      top = 78;	  
    }
    else if(button_y == 3){
      left = 3;
      top = 107;	  
    }
  }
  else{
    //右列
    if(button_y == 0){
      left = 32;
      top = 20;
    }
    else if(button_y == 1){
      left = 32;
      top = 49;
    }
    else if(button_y == 2){
      left = 32;
      top = 78;	  
    }
    else if(button_y == 3){
      left = 32;
      top = 107;	  
    }
  }

  toolImg = tool_normalImg.copy();

  for(int y=top; y<top+29; y++){
    for(int x=left; x<left+29; x++){
      toolImg.setPixel(x,y,tool_reverseImg.pixel(x,y));
    }
  }

  if(stickyFlag){
    for(int y=3; y<17; y++){
      for(int x=32; x<61; x++){
	toolImg.setPixel(x,y,tool_reverseImg.pixel(x,y));
      }
    }
  }
  else{
    for(int y=3; y<17; y++){
      for(int x=32; x<61; x++){
	toolImg.setPixel(x,y,tool_normalImg.pixel(x,y));
      }
    }
  }


  if(minimizePressFlag){
    for(int y=3; y<17; y++){
      for(int x=32; x<46; x++){
	toolImg.setPixel(x,y,(~(tool_reverseImg.pixel(x,y))) | 0xff000000);
      }
    }
  }
  else{
    if(stickyFlag){
      for(int y=3; y<17; y++){
	for(int x=32; x<46; x++){
	  toolImg.setPixel(x,y,((tool_reverseImg.pixel(x,y))) | 0xff000000);
	}
      }
    }
    else{
      for(int y=3; y<17; y++){
	for(int x=32; x<46; x++){
	  toolImg.setPixel(x,y,((tool_normalImg.pixel(x,y))) | 0xff000000);
	}
      }
    }

  }

  if(closePressFlag){
    for(int y=3; y<17; y++){
      for(int x=47; x<61; x++){
	toolImg.setPixel(x,y,(~(tool_reverseImg.pixel(x,y))) | 0xff000000);
      }
    }
  }
  else{
    if(stickyFlag){
      for(int y=3; y<17; y++){
	for(int x=47; x<61; x++){
	  toolImg.setPixel(x,y,((tool_reverseImg.pixel(x,y))) | 0xff000000);
	}
      }
    }
    else{
      for(int y=3; y<17; y++){
	for(int x=47; x<61; x++){
	  toolImg.setPixel(x,y,((tool_normalImg.pixel(x,y))) | 0xff000000);
	}
      }
    }

  }


  QPixmap toolPix = QPixmap();
  toolPix.convertFromImage(toolImg, 0);
  setBackgroundPixmap(toolPix);

}

void Tool::showEvent( QShowEvent *e ){
  this->setActiveWindow();
  this->raise();


  if(!stickyFlag && (this->y() != canvas->toolPanelY)){
    move(TOOLPANEL_X, canvas->toolPanelY);
  }
  myReset();
}

void Tool::hideEvent( QHideEvent *e ){
    for(int y=TOOLICON_Y; y<TOOLICON_Y+TOOLICON_HEIGHT; y++){
      for(int x=TOOLICON_X; x<TOOLICON_X+TOOLICON_WIDTH; x++){
        canvas->toolbar->toolbarImg.setPixel(x,y,canvas->toolbar->toolbar_normalImg.pixel(x,y));
      }
    }
    QPixmap toolbarPix = QPixmap();
    toolbarPix.convertFromImage(canvas->toolbar->toolbarImg, 0);
    canvas->toolbar->setBackgroundPixmap(toolbarPix);

}

void Tool::paintEvent( QPaintEvent *e ){
  // 他のダイアログとデッドロックを起こすので以下は利用しないこと
  /*

  */

}

void Tool::mousePressEvent( QMouseEvent *e )
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


  if(e->x() >= 3 && e->x() < 32){
    // 左列
    if(e->y() >= 20 && e->y() < 49){
      changeTool(0,0);
    }
    else if(e->y() >= 49 && e->y() < 78){
      changeTool(0,1);
    }
    else if(e->y() >= 78 && e->y()< 107){
      changeTool(0,2);
    }
    else if(e->y() >= 107 && e->y()< 136){
      changeTool(0,3);
    }
  }
  else if (e->x()>= 32 && e->x()< 61){
    // 右列
    if(e->y() >= 20 && e->y() < 49){
      changeTool(1,0);
    }
    else if(e->y() >= 49 && e->y() < 78){
      changeTool(1,1);
    }
    else if(e->y() >= 78 && e->y()< 107){
      changeTool(1,2);
    }    
    else if(e->y() >= 107 && e->y()< 136){
      changeTool(1,3);
    }    
  }


}


void Tool::mouseMoveEvent( QMouseEvent *e )
{
  int top=1, left=1;
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
      changeTool(0,0);
    }
    else if(e->y() >= 49 && e->y() < 78){
      changeTool(0,1);
    }
    else if(e->y() >= 78 && e->y()< 107){
      changeTool(0,2);
    }
    else if(e->y() >= 107 && e->y()< 136){
      changeTool(0,3);
    }
  }
  else if (e->x()>= 32 && e->x()< 61){
    // 右列
    if(e->y() >= 20 && e->y() < 49){
      changeTool(1,0);
    }
    else if(e->y() >= 49 && e->y() < 78){
      changeTool(1,1);
    }
    else if(e->y() >= 78 && e->y()< 107){
      changeTool(1,2);
    }    
    else if(e->y() >= 107 && e->y()< 136){
      changeTool(1,3);
    }    
  }

}


void Tool::mouseReleaseEvent( QMouseEvent *e ){

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
      canvas->fillFlag = false;
      canvas->bokeashiFlag = false;
      canvas->kasanenuriFlag = false;
      changeTool(0,0);
      canvas->tool = PEN;
      canvas->toolbar->changeToDrawtool();
    }
    else if(e->y() >= 49 && e->y() < 78){
      canvas->fillFlag = false;
      canvas->bokeashiFlag = true;
      canvas->kasanenuriFlag = false;
      changeTool(0,1);
      canvas->tool = PEN;
      canvas->toolbar->changeToDrawtool();
    }
    else if(e->y() >= 78 && e->y() < 107){

      canvas->fillFlag = false;
      canvas->bokeashiFlag = true;
      canvas->kasanenuriFlag = true;
      changeTool(0,2);
      canvas->tool = PEN;
      canvas->toolbar->changeToDrawtool();

    }
    else if(e->y() >= 107 && e->y()< 136){
      canvas->bokeashiFlag = false;
      canvas->kasanenuriFlag = false;
      changeTool(0,3);
      canvas->tool = PEN;
      canvas->fillFlag = true;
      canvas->toolbar->changeToDrawtool();
    }
  }
  else if (e->x()>= 32 && e->x()< 61){
    // 右列
    if(e->y() >= 20 && e->y() < 49){
      canvas->fillFlag = false;
      canvas->bokeashiFlag = false;
      canvas->kasanenuriFlag = false;
      changeTool(1,0);
      canvas->tool = ERASER;
      canvas->toolbar->changeToEraser();
    }
    if(e->y() >= 49 && e->y() < 78){
      canvas->fillFlag = false;
      canvas->bokeashiFlag = true;
      canvas->kasanenuriFlag = false;
      changeTool(1,1);
      canvas->tool = ERASER;
      canvas->toolbar->changeToEraser();
    }
    else if(e->y() >= 78 && e->y() < 107){
      canvas->fillFlag = false;
      canvas->bokeashiFlag = false;
      canvas->kasanenuriFlag = false;
      changeTool(1,2);
      canvas->tool = BLUR;
      canvas->powerBlurFlag = false;
      canvas->toolbar->changeToBlur();

    }
    else if(e->y() >= 107 && e->y()< 136){

      canvas->fillFlag = false;
      canvas->bokeashiFlag = false;
      canvas->kasanenuriFlag = false;
      changeTool(1,3);
      canvas->tool = BLUR;
      canvas->powerBlurFlag = true;
      canvas->toolbar->changeToBlur();
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
      this->move(TOOLPANEL_X,canvas->toolPanelY);
    }
  }

  if(!stickyFlag){
     this->hide();
     display = false;
  }

  myReset();

  canvas->toolbar->redrawToolBar();

}
