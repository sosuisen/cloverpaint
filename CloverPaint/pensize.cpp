#include "clover.h"


#define PENSIZEICON_X 68
#define PENSIZEICON_Y 2
#define PENSIZEICON_WIDTH 29
#define PENSIZEICON_HEIGHT 29



PenSize::PenSize(Canvas * parent, const char * name, bool modal, WFlags f)
  :QDialog(parent, name, modal, f)
{
  canvas = parent;

  moveFlag = false;
  stickyFlag = false;
  minimizePressFlag = false;
  closePressFlag = false;

  display = false;

  reparent( parent, QPoint(PENSIZEPANEL_X, canvas->pensizePanelY));
  resize(65, 198);

  pensizeImg = new QImage(*(canvas->pensize_normalImg));

  //ツールバー画像の表示
  QPixmap *pensizePix = new QPixmap();
  pensizePix->convertFromImage(*pensizeImg, 0);
  setBackgroundPixmap(*pensizePix);
  delete pensizePix;
}

PenSize::~PenSize()
{
  delete pensizeImg;
}

void PenSize::myReset(){

  int toolsize=0;
  
  switch(canvas->tool){

  case PEN:
    toolsize=canvas->pensize;
    break;
  case ERASER:
    toolsize=canvas->erasersize;
    break;
  case BLUR:
    toolsize=canvas->blursize;
    break;
  case SHARPEN:
    toolsize=canvas->sharpensize;
    break;

  }
  
  changePenSize(toolsize);


}

void PenSize::showEvent( QShowEvent *e ){
  this->setActiveWindow();
  this->raise();


  if(!stickyFlag && (this->y() != canvas->pensizePanelY)){
    move(PENSIZEPANEL_X, canvas->pensizePanelY);
  }
  myReset();
}

void PenSize::hideEvent( QHideEvent *e ){

}

void PenSize::paintEvent( QPaintEvent *e ){
  // 他のダイアログとデッドロックを起こすので以下は利用しないこと
  /*
  */

}

void PenSize::changePenSize(int size){

  int left=0;
  int top=0;

  short *toolsize = NULL;
  switch(canvas->tool){
  case PEN:
    toolsize = &(canvas->pensize);
    break;
  case ERASER:
    toolsize = &(canvas->erasersize);
    break;
  case BLUR:
    toolsize = &(canvas->blursize);
    break;
  case SHARPEN:
    toolsize = &(canvas->sharpensize);
    break;

  }

  *toolsize = size;


    switch(size){
    case 11: 
      canvas->brushImage = canvas->brush11;
      break;
    case 17: 
      canvas->brushImage = canvas->brush17;
      break;
    case 25:
      canvas->brushImage = canvas->brush25;
      break;
    case 33: 
      canvas->brushImage = canvas->brush33;
      break;
    case 65: 
      canvas->brushImage = canvas->brush65;
      break;
    case 99: 
      canvas->brushImage = canvas->brush99;
      break;
    }

  switch(size){
  case 1:
    left = 32;
    top = 20;
    break;
  case 2:
    left = 32;
    top = 49;
    break;
  case 3:
    left = 32;
    top = 78;	  
    break;
  case 4:
    left = 32;
    top = 107;	  
    break;
  case 5:
    left = 32;
    top = 136;
    break;
  case 7:
    left = 32;
    top = 165;
    break;
  case 11:
    left = 3;
    top = 20;	  
    break;
  case 17:
    left = 3;
    top = 49;
    break;
  case 25:
    left = 3;
    top = 78;	  
    break;
  case 33:
    left = 3;
    top = 107;	  
    break;
  case 65:
    left = 3;
    top = 136;	  
    break;
  case 99:
    left = 3;
    top = 165;
    break;
  default:
    break;
  }
  
  *pensizeImg = canvas->pensize_normalImg->copy();

  for(int y=top; y<top+29; y++){
    for(int x=left; x<left+29; x++){
      pensizeImg->setPixel(x,y,canvas->pensize_reverseImg->pixel(x,y));
    }
  }


  if(stickyFlag){
    for(int y=3; y<17; y++){
      for(int x=32; x<61; x++){
	pensizeImg->setPixel(x,y,canvas->pensize_reverseImg->pixel(x,y));
      }
    }
  }
  else{
    for(int y=3; y<17; y++){
      for(int x=32; x<61; x++){
	pensizeImg->setPixel(x,y,canvas->pensize_normalImg->pixel(x,y));
      }
    }
  }

    if(minimizePressFlag){
      for(int y=3; y<17; y++){
	for(int x=32; x<46; x++){
	  pensizeImg->setPixel(x,y,(~(canvas->pensize_reverseImg->pixel(x,y))) | 0xff000000);
	}
      }
    }
    else{
      if(stickyFlag){
	for(int y=3; y<17; y++){
	  for(int x=32; x<46; x++){
	    pensizeImg->setPixel(x,y,((canvas->pensize_reverseImg->pixel(x,y))) | 0xff000000);
	  }
	}
      }
      else{
	for(int y=3; y<17; y++){
	  for(int x=32; x<46; x++){
	    pensizeImg->setPixel(x,y,((canvas->pensize_normalImg->pixel(x,y))) | 0xff000000);
	  }
	}
      }
    }

    if(closePressFlag){
      for(int y=3; y<17; y++){
	for(int x=47; x<61; x++){
	  pensizeImg->setPixel(x,y,(~(canvas->pensize_reverseImg->pixel(x,y))) | 0xff000000);
	}
      }
    }
    else{
      if(stickyFlag){
	for(int y=3; y<17; y++){
	  for(int x=47; x<61; x++){
	    pensizeImg->setPixel(x,y,((canvas->pensize_reverseImg->pixel(x,y))) | 0xff000000);
	  }
	}
      }
      else{
	for(int y=3; y<17; y++){
	  for(int x=47; x<61; x++){
	    pensizeImg->setPixel(x,y,((canvas->pensize_normalImg->pixel(x,y))) | 0xff000000);
	  }
	}
      }
    }


  QPixmap *pensizePix = new QPixmap();
  pensizePix->convertFromImage(*pensizeImg, 0);
  setBackgroundPixmap(*pensizePix);
  delete pensizePix;
}



void PenSize::mousePressEvent( QMouseEvent *e )
{
  int toolsize=0;
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

      switch(canvas->tool){
      case PEN:
	toolsize=canvas->pensize;
	break;
      case ERASER:
	toolsize=canvas->erasersize;
	break;
      case BLUR:
	toolsize=canvas->blursize;
	break;
      case SHARPEN:
	toolsize=canvas->sharpensize;
	break;
      }
      changePenSize(toolsize);

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
      changePenSize(11);
    }
    else if(e->y() >= 49 && e->y() < 78){
      changePenSize(17);
    }
    else if(e->y() >= 78 && e->y()< 107){
      changePenSize(25);
    }
    else if(e->y() >= 107 && e->y()< 136){
      changePenSize(33);
    }
    else if(e->y() >= 136 && e->y()< 165){
      changePenSize(65);
    }
    else if(e->y() >= 165 && e->y()< 194){
      changePenSize(99);
    }
  }
  else if (e->x()>= 32 && e->x()< 61){
    // 右列
    if(e->y() >= 20 && e->y() < 49){
      changePenSize(1);
    }
    else if(e->y() >= 49 && e->y() < 78){
      changePenSize(2);
    }
    else if(e->y() >= 78 && e->y()< 107){
      changePenSize(3);
    }    
    else if(e->y() >= 107 && e->y()< 136){
      changePenSize(4);
    }    
    else if(e->y() >= 136 && e->y()< 165){
      changePenSize(5);
    }    
    else if(e->y() >= 165 && e->y()< 194){
      changePenSize(7);
    }    
  }



}


void PenSize::mouseMoveEvent( QMouseEvent *e )
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
      changePenSize(11);
    }
    else if(e->y() >= 49 && e->y() < 78){
      changePenSize(17);
    }
    else if(e->y() >= 78 && e->y()< 107){
      changePenSize(25);
    }
    else if(e->y() >= 107 && e->y()< 136){
      changePenSize(33);
    }
    else if(e->y() >= 136 && e->y()< 165){
      changePenSize(65);
    }
    else if(e->y() >= 165 && e->y()< 194){
      changePenSize(99);
    }
  }
  else if (e->x()>= 32 && e->x()< 61){
    // 右列
    if(e->y() >= 20 && e->y() < 49){
      changePenSize(1);
    }
    else if(e->y() >= 49 && e->y() < 78){
      changePenSize(2);
    }
    else if(e->y() >= 78 && e->y()< 107){
      changePenSize(3);
    }    
    else if(e->y() >= 107 && e->y()< 136){
      changePenSize(4);
    }    
    else if(e->y() >= 136 && e->y()< 165){
      changePenSize(5);
    }    
    else if(e->y() >= 165 && e->y()< 194){
      changePenSize(7);
    }    
  }

}



void PenSize::mouseReleaseEvent( QMouseEvent *e ){

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
      this->move(PENSIZEPANEL_X,canvas->pensizePanelY);
    }
  }

  if(!stickyFlag){
     this->hide();
      display = false;
  }

  myReset();

  canvas->toolbar->redrawToolBar();


}
