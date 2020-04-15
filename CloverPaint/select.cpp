#include "clover.h"
#include "select_normal.h"
#include "select_reverse.h"


Select::Select(Canvas * parent, const char * name, bool modal, WFlags f)
  :QDialog(parent, name, modal, f)
{
  canvas = parent;

  moveFlag = false;
  stickyFlag = false;
  minimizePressFlag = false;
  closePressFlag = false;
  display = false;

  resize(65, 124);

  reparent( parent, QPoint(SELECTPANEL_X, 640-31-20-this->height()));

  //画像の読み込み
  QImage tmpImg = QImage(select_normal);
  //xpmからの読みこみはインデックスカラーになるため変換
  selectImg = tmpImg.convertDepth(32);
  select_normalImg = tmpImg.convertDepth(32);

  tmpImg = QImage(select_reverse);
  select_reverseImg = tmpImg.convertDepth(32);

  //画像の表示
  QPixmap selectPix = QPixmap();
  selectPix.convertFromImage(selectImg, 0);
  setBackgroundPixmap(selectPix);

}

Select::~Select()
{
}


void Select::myReset(){

  for(int y=0; y<this->height(); y++){
    for(int x=0; x<this->width(); x++){
      selectImg.setPixel(x,y,select_normalImg.pixel(x,y));
    }
  }


  if(canvas->moveRectFlag){
    for(int y=20; y<20+29; y++){
      for(int x=32; x<32+29; x++){
	selectImg.setPixel(x,y,select_reverseImg.pixel(x,y));
      }
    }
  }
  else{
    for(int y=20; y<20+29; y++){
      for(int x=32; x<32+29; x++){
	selectImg.setPixel(x,y,select_normalImg.pixel(x,y));
      }
    }
  }


  if(stickyFlag){
    for(int y=3; y<17; y++){
      for(int x=32; x<61; x++){
	selectImg.setPixel(x,y,select_reverseImg.pixel(x,y));
      }
    }
  }
  else{
    for(int y=3; y<17; y++){
      for(int x=32; x<61; x++){
	selectImg.setPixel(x,y,select_normalImg.pixel(x,y));
      }
    }
  }

    if(minimizePressFlag){
      for(int y=3; y<17; y++){
	for(int x=32; x<46; x++){
	  selectImg.setPixel(x,y,(~(select_reverseImg.pixel(x,y))) | 0xff000000);
	}
      }
    }
    else{
      if(stickyFlag){
	for(int y=3; y<17; y++){
	  for(int x=32; x<46; x++){
	    selectImg.setPixel(x,y,((select_reverseImg.pixel(x,y))) | 0xff000000);
	  }
	}
      }
      else{
	for(int y=3; y<17; y++){
	  for(int x=32; x<46; x++){
	    selectImg.setPixel(x,y,((select_normalImg.pixel(x,y))) | 0xff000000);
	  }
	}
      }

    }

    if(closePressFlag){
      for(int y=3; y<17; y++){
	for(int x=47; x<61; x++){
	  selectImg.setPixel(x,y,(~(select_reverseImg.pixel(x,y))) | 0xff000000);
	}
      }
    }
    else{
      if(stickyFlag){
	for(int y=3; y<17; y++){
	  for(int x=47; x<61; x++){
	    selectImg.setPixel(x,y,((select_reverseImg.pixel(x,y))) | 0xff000000);
	  }
	}
      }
      else{
	for(int y=3; y<17; y++){
	  for(int x=47; x<61; x++){
	    selectImg.setPixel(x,y,((select_normalImg.pixel(x,y))) | 0xff000000);
	  }
	}
      }

    }


  QPixmap selectPix = QPixmap();
  selectPix.convertFromImage(selectImg, 0);
  setBackgroundPixmap(selectPix);


}


void Select::showEvent( QShowEvent *e ){
  this->setActiveWindow();
  this->raise();

  /*  for(int y=SELECTICON_Y; y<SELECTICON_Y+SELECTICON_HEIGHT; y++){
    for(int x=SELECTICON_X; x<SELECTICON_X+SELECTICON_WIDTH; x++){
      canvas->toolbar->toolbarImg.setPixel(x,y,canvas->toolbar->toolbar_reverseImg.pixel(x,y));
    }
  }
  QPixmap toolbarPix = QPixmap();
  toolbarPix.convertFromImage(canvas->toolbar->toolbarImg, 0);
  canvas->toolbar->setBackgroundPixmap(toolbarPix);
  */

  if(!stickyFlag && (this->y() != canvas->selectPanelY)){
    move(SELECTPANEL_X, canvas->selectPanelY);
  }
  myReset();
}

void Select::hideEvent( QHideEvent *e ){
  /*    for(int y=SELECTICON_Y; y<SELECTICON_Y+SELECTICON_HEIGHT; y++){
      for(int x=SELECTICON_X; x<SELECTICON_X+SELECTICON_WIDTH; x++){
        canvas->toolbar->toolbarImg.setPixel(x,y,canvas->toolbar->toolbar_normalImg.pixel(x,y));
      }
    }
    QPixmap toolbarPix = QPixmap();
    toolbarPix.convertFromImage(canvas->toolbar->toolbarImg, 0);
    canvas->toolbar->setBackgroundPixmap(toolbarPix);
  */
}

void Select::paintEvent( QPaintEvent *e ){
  // 他のダイアログとデッドロックを起こすので以下は利用しないこと
  /*

  */

}


void Select::mousePressEvent( QMouseEvent *e )
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


  for(int y=0; y<this->height(); y++){
    for(int x=0; x<this->width(); x++){
      selectImg.setPixel(x,y,select_normalImg.pixel(x,y));
    }
  }
  if(e->y()<78){
    if(e->x() >= 3 && e->x() < 32){
      // 左列
      if(e->y() >= 20 && e->y() < 49){
      }
      else if(e->y() >= 49 && e->y() < 78){
	//左右反転
	for(int y=49; y<49+29; y++){
	  for(int x=3; x<3+29; x++){
	    selectImg.setPixel(x,y,select_reverseImg.pixel(x,y));
	  }
	}
      }

    }
    else if (e->x()>= 32 && e->x()< 61){
      // 右列
      if(e->y() >= 20 && e->y() < 49){
	//移動ツール
	for(int y=20; y<20+29; y++){
	  for(int x=32; x<32+29; x++){
	    selectImg.setPixel(x,y,select_reverseImg.pixel(x,y));
	  }
	}
      }
      else if(e->y() >= 49 && e->y() < 78){
	//上下反転
	for(int y=49; y<49+29; y++){
	  for(int x=32; x<32+29; x++){
	    selectImg.setPixel(x,y,select_reverseImg.pixel(x,y));
	  }
	}
      }
    }
  }
  else{
    //矢印
    if(e->x() < 20 && e->y() > 89 && e->y() < 110){
      //左
      for(int y=89; y<110; y++){
	for(int x=0; x<20; x++){
	  selectImg.setPixel(x,y,select_reverseImg.pixel(x,y));
	}
      }
    }
    else if(e->x() > 41 && e->y() > 89 && e->y() < 110){
      //右
      for(int y=89; y<110; y++){
	for(int x=41; x<this->width(); x++){
	  selectImg.setPixel(x,y,select_reverseImg.pixel(x,y));
	}
      }
    }
    else if(e->x() > 20 &&  e->x() < 41 && e->y() > 79 && e->y() < 98){
      //上
      for(int y=79; y<98; y++){
	for(int x=20; x<41; x++){
	  selectImg.setPixel(x,y,select_reverseImg.pixel(x,y));
	}
      }
    }
    else if(e->x() > 20 &&  e->x() < 41 && e->y() > 101){
      //下
      for(int y=101; y<this->height(); y++){
	for(int x=20; x<41; x++){
	  selectImg.setPixel(x,y,select_reverseImg.pixel(x,y));
	}
      }
    }
    

  }

  if(stickyFlag){
    for(int y=3; y<17; y++){
      for(int x=32; x<61; x++){
	selectImg.setPixel(x,y,select_reverseImg.pixel(x,y));
      }
    }
  }
  else{
    for(int y=3; y<17; y++){
      for(int x=32; x<61; x++){
	selectImg.setPixel(x,y,select_normalImg.pixel(x,y));
      }
    }
  }

  if(canvas->moveRectFlag){
    for(int y=20; y<20+29; y++){
      for(int x=32; x<32+29; x++){
	selectImg.setPixel(x,y,select_reverseImg.pixel(x,y));
      }
    }
  }
  else{
    for(int y=20; y<20+29; y++){
      for(int x=32; x<32+29; x++){
	selectImg.setPixel(x,y,select_normalImg.pixel(x,y));
      }
    }
  }

  QPixmap selectPix = QPixmap();
  selectPix.convertFromImage(selectImg, 0);
  setBackgroundPixmap(selectPix);

}


void Select::mouseMoveEvent( QMouseEvent *e )
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



  for(int y=0; y<this->height(); y++){
    for(int x=0; x<this->width(); x++){
      selectImg.setPixel(x,y,select_normalImg.pixel(x,y));
    }
  }
  if(e->y()<78){
    if(e->x() >= 3 && e->x() < 32){
      // 左列
      if(e->y() >= 20 && e->y() < 49){
      }
      else if(e->y() >= 49 && e->y() < 78){
	//左右反転
	for(int y=49; y<49+29; y++){
	  for(int x=3; x<3+29; x++){
	    selectImg.setPixel(x,y,select_reverseImg.pixel(x,y));
	  }
	}
      }

    }
    else if (e->x()>= 32 && e->x()< 61){
      // 右列
      if(e->y() >= 20 && e->y() < 49){
	//移動ツール
	for(int y=20; y<20+29; y++){
	  for(int x=32; x<32+29; x++){
	    selectImg.setPixel(x,y,select_reverseImg.pixel(x,y));
	  }
	}
      }
      else if(e->y() >= 49 && e->y() < 78){
	//上下反転
	for(int y=49; y<49+29; y++){
	  for(int x=32; x<32+29; x++){
	    selectImg.setPixel(x,y,select_reverseImg.pixel(x,y));
	  }
	}
      }
    }
  }
  else{
    //矢印
    if(e->x() < 20 && e->y() > 89 && e->y() < 110){
      //左
      for(int y=89; y<110; y++){
	for(int x=0; x<20; x++){
	  selectImg.setPixel(x,y,select_reverseImg.pixel(x,y));
	}
      }
    }
    else if(e->x() > 41 && e->y() > 89 && e->y() < 110){
      //右
      for(int y=89; y<110; y++){
	for(int x=41; x<this->width(); x++){
	  selectImg.setPixel(x,y,select_reverseImg.pixel(x,y));
	}
      }
    }
    else if(e->x() > 20 &&  e->x() < 41 && e->y() > 79 && e->y() < 98){
      //上
      for(int y=79; y<98; y++){
	for(int x=20; x<41; x++){
	  selectImg.setPixel(x,y,select_reverseImg.pixel(x,y));
	}
      }
    }
    else if(e->x() > 20 &&  e->x() < 41 && e->y() > 101){
      //下
      for(int y=101; y<this->height(); y++){
	for(int x=20; x<41; x++){
	  selectImg.setPixel(x,y,select_reverseImg.pixel(x,y));
	}
      }
    }
  }

  if(stickyFlag){
    for(int y=3; y<17; y++){
      for(int x=32; x<61; x++){
	selectImg.setPixel(x,y,select_reverseImg.pixel(x,y));
      }
    }
  }
  else{
    for(int y=3; y<17; y++){
      for(int x=32; x<61; x++){
	selectImg.setPixel(x,y,select_normalImg.pixel(x,y));
      }
    }
  }

  if(canvas->moveRectFlag){
    for(int y=20; y<20+29; y++){
      for(int x=32; x<32+29; x++){
	selectImg.setPixel(x,y,select_reverseImg.pixel(x,y));
      }
    }
  }
  else{
    for(int y=20; y<20+29; y++){
      for(int x=32; x<32+29; x++){
	selectImg.setPixel(x,y,select_normalImg.pixel(x,y));
      }
    }
  }

  QPixmap selectPix = QPixmap();
  selectPix.convertFromImage(selectImg, 0);
  setBackgroundPixmap(selectPix);

}



void Select::mouseReleaseEvent( QMouseEvent *e ){

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


  if(e->y()<78){
    if(e->x() >= 3 && e->x() < 32){
      // 左列
      if(e->y() >= 20 && e->y() < 49){
      }
      else if(e->y() >= 49 && e->y() < 78){
	//左右反転

	if(canvas->canvasType == VGA){

	  if(canvas->moveRectFlag){
	    canvas->moveRectX = -canvas->moveRectX;
	  }

	  unsigned int tmpColor;

	  int tmpLeft = canvas->undo_left;
	  canvas->undo_left = 479-canvas->undo_right;
	  canvas->undo_right = 479-tmpLeft;

	  for(int y=0; y<640; y++){
	    for(int x=0; x<240; x++){
	      tmpColor = canvas->layer[canvas->currentlayer]->pixel(x,y);
	      canvas->layer[canvas->currentlayer]->setPixel(x,y, canvas->layer[canvas->currentlayer]->pixel(479-x, y));
	      canvas->layer[canvas->currentlayer]->setPixel(479-x,y,tmpColor);
	    
	      tmpColor = *(canvas->undoBuffer+x+y*480);
	      *(canvas->undoBuffer+x+y*480) = *(canvas->undoBuffer+(479-x)+y*480);
	      *(canvas->undoBuffer+(479-x)+y*480) = tmpColor;

	      tmpColor = *(canvas->redoBuffer+x+y*480);
	      *(canvas->redoBuffer+x+y*480) = *(canvas->redoBuffer+(479-x)+y*480);
	      *(canvas->redoBuffer+(479-x)+y*480) = tmpColor;
	    }
	  }
	}
	else if(canvas->canvasType == QVGA){

	  if(canvas->moveRectFlag){
	    canvas->moveRectX = -canvas->moveRectX;
	  }

	  unsigned int tmpColor;

	  int tmpLeft = canvas->undo_left;
	  canvas->undo_left = 239-canvas->undo_right;
	  canvas->undo_right = 239-tmpLeft;

	  for(int y=0; y<320; y++){
	    for(int x=0; x<120; x++){
	      tmpColor = canvas->layer[canvas->currentlayer]->pixel(x,y);
	      canvas->layer[canvas->currentlayer]->setPixel(x,y, canvas->layer[canvas->currentlayer]->pixel(239-x, y));
	      canvas->layer[canvas->currentlayer]->setPixel(239-x,y,tmpColor);
	    
	      tmpColor = *(canvas->undoBuffer+x+y*480);
	      *(canvas->undoBuffer+x+y*480) = *(canvas->undoBuffer+(239-x)+y*480);
	      *(canvas->undoBuffer+(239-x)+y*480) = tmpColor;

	      tmpColor = *(canvas->redoBuffer+x+y*480);
	      *(canvas->redoBuffer+x+y*480) = *(canvas->redoBuffer+(239-x)+y*480);
	      *(canvas->redoBuffer+(239-x)+y*480) = tmpColor;
	    }
	  }
	}




	canvas->repaint();
      }

    }
    else if (e->x()>= 32 && e->x()< 61){
      // 右列
      if(e->y() >= 20 && e->y() < 49){
	//移動ツール
	if(!canvas->moveRectFlag){
	  for(int y=0; y<640; y++){
	    for(int x=0; x<480; x++){
	      *(canvas->undoBuffer+x+y*480) = canvas->layer[canvas->currentlayer]->pixel(x,y);
	    }
	  }
	  canvas->moveRectX = 0;
	  canvas->moveRectY = 0;
	  canvas->moveRectFlag = true;
	}
	else{
	  for(int y=0; y<640; y++){
	    for(int x=0; x<480; x++){
	      *(canvas->undoBuffer+x+y*480) = canvas->layer[canvas->currentlayer]->pixel(x,y);
	    }
	  }
	  canvas->moveRectFlag = false;
	}
	stickyFlag = true;
	canvas->undoFlag = false;
	canvas->toolbar->redrawToolBar();

      }
      else if(e->y() >= 49 && e->y() < 78){
	//上下反転

	if(canvas->canvasType == VGA){
	  if(canvas->moveRectFlag){
	    canvas->moveRectY = -canvas->moveRectY;
	  }

	  unsigned int tmpColor;

	  int tmpTop = canvas->undo_top;
	  canvas->undo_top = 639-canvas->undo_bottom;
	  canvas->undo_bottom = 639-tmpTop;

	  for(int y=0; y<320; y++){
	    for(int x=0; x<480; x++){
	      tmpColor = canvas->layer[canvas->currentlayer]->pixel(x,y);
	      canvas->layer[canvas->currentlayer]->setPixel(x,y, canvas->layer[canvas->currentlayer]->pixel(x, 639-y));
	      canvas->layer[canvas->currentlayer]->setPixel(x, 639-y,tmpColor);

	      tmpColor = *(canvas->undoBuffer+x+y*480);
	      *(canvas->undoBuffer+x+y*480) = *(canvas->undoBuffer+(x)+(639-y)*480);
	      *(canvas->undoBuffer+(x)+(639-y)*480) = tmpColor;
	    
	      tmpColor = *(canvas->redoBuffer+x+y*480);
	      *(canvas->redoBuffer+x+y*480) = *(canvas->redoBuffer+(x)+(639-y)*480);
	      *(canvas->redoBuffer+(x)+(639-y)*480) = tmpColor;
	    }
	  }
	}
	else if(canvas->canvasType == QVGA){
	  if(canvas->moveRectFlag){
	    canvas->moveRectY = -canvas->moveRectY;
	  }

	  unsigned int tmpColor;

	  int tmpTop = canvas->undo_top;
	  canvas->undo_top = 319-canvas->undo_bottom;
	  canvas->undo_bottom = 319-tmpTop;

	  for(int y=0; y<160; y++){
	    for(int x=0; x<240; x++){
	      tmpColor = canvas->layer[canvas->currentlayer]->pixel(x,y);
	      canvas->layer[canvas->currentlayer]->setPixel(x,y, canvas->layer[canvas->currentlayer]->pixel(x, 319-y));
	      canvas->layer[canvas->currentlayer]->setPixel(x, 319-y,tmpColor);

	      tmpColor = *(canvas->undoBuffer+x+y*480);
	      *(canvas->undoBuffer+x+y*480) = *(canvas->undoBuffer+(x)+(319-y)*480);
	      *(canvas->undoBuffer+(x)+(319-y)*480) = tmpColor;
	    
	      tmpColor = *(canvas->redoBuffer+x+y*480);
	      *(canvas->redoBuffer+x+y*480) = *(canvas->redoBuffer+(x)+(319-y)*480);
	      *(canvas->redoBuffer+(x)+(319-y)*480) = tmpColor;
	    }
	  }
	}


	canvas->repaint();
      }
    }
  }
  else{
    //矢印
    if(e->x() < 20 && e->y() > 89 && e->y() < 110){
      //左
      if(!canvas->moveRectFlag){
	for(int y=0; y<640; y++){
	  for(int x=0; x<480; x++){
	    *(canvas->undoBuffer+x+y*480) = canvas->layer[canvas->currentlayer]->pixel(x,y);
	  }
	}
      }

	stickyFlag = true;
	canvas->moveRectFlag = true;
	canvas->undoFlag = false;
	canvas->toolbar->redrawToolBar();

	for(int y=0; y<640; y++){
	  for(int x=0; x<479; x++){
	    canvas->layer[canvas->currentlayer]->setPixel(x,y,canvas->layer[canvas->currentlayer]->pixel(x+1,y));
	  }
	}
	if(canvas->currentlayer == 0){
	  for(int y=0; y<640; y++){
	    canvas->layer[canvas->currentlayer]->setPixel(479,y, 0xffffffff);
	  }
	}
	else{
	  for(int y=0; y<640; y++){
	    canvas->layer[canvas->currentlayer]->setPixel(479,y, 0x00ffffff);
	  }
	}

	canvas->repaint();

    }
    else if(e->x() > 41 && e->y() > 89 && e->y() < 110){
      //右
      if(!canvas->moveRectFlag){
	for(int y=0; y<640; y++){
	  for(int x=0; x<480; x++){
	    *(canvas->undoBuffer+x+y*480) = canvas->layer[canvas->currentlayer]->pixel(x,y);
	  }
	}
      }
	stickyFlag = true;
	canvas->moveRectFlag = true;
	canvas->undoFlag = false;
	canvas->toolbar->redrawToolBar();

	for(int y=0; y<640; y++){
	  for(int x=479; x>0; x--){
	    canvas->layer[canvas->currentlayer]->setPixel(x,y, canvas->layer[canvas->currentlayer]->pixel(x-1,y));
	  }
	}

	if(canvas->currentlayer == 0){
	  for(int y=0; y<640; y++){
	    canvas->layer[canvas->currentlayer]->setPixel(0,y, 0xffffffff);
	  }
	}
	else{
	  for(int y=0; y<640; y++){
	    canvas->layer[canvas->currentlayer]->setPixel(0,y, 0x00ffffff);
	  }
	}

	canvas->repaint();

    }
    else if(e->x() > 20 &&  e->x() < 41 && e->y() > 79 && e->y() < 98){
      //上
      if(!canvas->moveRectFlag){
	for(int y=0; y<640; y++){
	  for(int x=0; x<480; x++){
	    *(canvas->undoBuffer+x+y*480) = canvas->layer[canvas->currentlayer]->pixel(x,y);
	  }
	}
      }
	stickyFlag = true;
	canvas->moveRectFlag = true;
	canvas->undoFlag = false;
	canvas->toolbar->redrawToolBar();

	for(int y=0; y<639; y++){
	  for(int x=0; x<480; x++){
	    canvas->layer[canvas->currentlayer]->setPixel(x,y, canvas->layer[canvas->currentlayer]->pixel(x,y+1));
	  }
	}

	if(canvas->currentlayer == 0){
	  for(int x=0; x<480; x++){
	    canvas->layer[canvas->currentlayer]->setPixel(x,639, 0xffffffff);
	  }
	}
	else{
	  for(int x=0; x<480; x++){
	    canvas->layer[canvas->currentlayer]->setPixel(x,639, 0x00ffffff);
	  }
	}
	canvas->repaint();
    }
    else if(e->x() > 20 &&  e->x() < 41 && e->y() > 101){
      //下
      if(!canvas->moveRectFlag){
	for(int y=0; y<640; y++){
	  for(int x=0; x<480; x++){
	    *(canvas->undoBuffer+x+y*480) = canvas->layer[canvas->currentlayer]->pixel(x,y);
	  }
	}
      }
	stickyFlag = true;
	canvas->moveRectFlag = true;
	canvas->undoFlag = false;
	canvas->toolbar->redrawToolBar();

	for(int y=639; y>0; y--){
	  for(int x=0; x<480; x++){
	    canvas->layer[canvas->currentlayer]->setPixel(x,y,canvas->layer[canvas->currentlayer]->pixel(x,y-1) );
	  }
	}

	if(canvas->currentlayer == 0){
	  for(int x=0; x<480; x++){
	    canvas->layer[canvas->currentlayer]->setPixel(x,0, 0xffffffff);
	  }
	}
	else{
	  for(int x=0; x<480; x++){
	    canvas->layer[canvas->currentlayer]->setPixel(x,0, 0x00ffffff);
	  }
	}
	canvas->repaint();
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
      this->move(SELECTPANEL_X,canvas->selectPanelY);
    }
  }

  if(!stickyFlag){
     this->hide();
      display = false;
  }

  myReset();

  canvas->toolbar->redrawToolBar();


}
