#include "clover.h"
#include "hue.h"

#include <math.h>

Hue::Hue(Canvas * parent, const char * name, bool modal, WFlags f)
  :QDialog(parent, name, modal, f)
{
  QPainter p;
  canvas = parent;

  reparent( parent, QPoint(0, canvas->huebarY));
  resize(480, 20);

  //ツールバー画像の読み込み
  QImage tmpImg = QImage(hue);
  //xpmからの読みこみはインデックスカラーになるため変換
  hueImg = tmpImg.convertDepth(32);

  //ツールバー画像の表示
  QPixmap huePix = QPixmap();
  huePix.convertFromImage(hueImg, 0);
  setBackgroundPixmap(huePix);

  hueImg.setPixel(479,2,(~(hueImg.pixel(479,2))) | 0xff000000);
  hueImg.setPixel(479,8,(~(hueImg.pixel(479,8))) | 0xff000000);

  hueImg.setPixel(478,2, (~(hueImg.pixel(479,2))) | 0xff000000);
  hueImg.setPixel(478,3, (~(hueImg.pixel(479,3))) | 0xff000000);
  hueImg.setPixel(478,4, (~(hueImg.pixel(479,4))) | 0xff000000);
  hueImg.setPixel(478,5, (~(hueImg.pixel(479,5))) | 0xff000000);
  hueImg.setPixel(478,6, (~(hueImg.pixel(479,6))) |  0xff000000);
  hueImg.setPixel(478,7, (~(hueImg.pixel(479,7))) | 0xff000000);

  repaint_left = 477;
  repaint_top = 0;
  repaint_width = 3;
  repaint_height = 20;
  canvas->toolbar->repaintColor(repaint_left, repaint_top, repaint_width, repaint_height, false);

  prevHuePoint = 0;

}

Hue::~Hue()
{
}



void Hue::paintEvent( QPaintEvent * )
{
      int repaint_left = 479-(int)(479*canvas->fgHu/COLORDIV);
      if(repaint_left < 0){
	repaint_left = 0;
      }
      else if(repaint_left > 477){
	repaint_left = 477;
      }
      int repaint_top = 0;
      int repaint_width = 3;
      int repaint_height = 20;
      canvas->toolbar->repaintColor(repaint_left, repaint_top, repaint_width, repaint_height, false);

}


void Hue::changeHueArea(int hue, int huePoint){

  if(hue > COLORDIV){
    hue = COLORDIV;
  }
  else if(hue < 0){
    hue = 0;
  }


  hueImg.setPixel(479-prevHuePoint,2,(~(hueImg.pixel(479-prevHuePoint,2))) | 0xff000000);
  hueImg.setPixel(479-prevHuePoint,8,(~(hueImg.pixel(479-prevHuePoint,8))) | 0xff000000);

  if(479-prevHuePoint!=479){
    hueImg.setPixel(479-prevHuePoint+1,2,(~(hueImg.pixel(479-prevHuePoint+1,2))) | 0xff000000);
    hueImg.setPixel(479-prevHuePoint+1,3,(~(hueImg.pixel(479-prevHuePoint+1,3))) | 0xff000000);
    hueImg.setPixel(479-prevHuePoint+1,4,(~(hueImg.pixel(479-prevHuePoint+1,4))) | 0xff000000);
    hueImg.setPixel(479-prevHuePoint+1,5,(~(hueImg.pixel(479-prevHuePoint+1,5))) | 0xff000000);
    hueImg.setPixel(479-prevHuePoint+1,6,(~(hueImg.pixel(479-prevHuePoint+1,6))) | 0xff000000);
    hueImg.setPixel(479-prevHuePoint+1,7,(~(hueImg.pixel(479-prevHuePoint+1,7))) | 0xff000000);
  }

  if(479-prevHuePoint!=0){
    hueImg.setPixel(479-prevHuePoint-1,2,(~(hueImg.pixel(479-prevHuePoint-1,2))) | 0xff000000);
    hueImg.setPixel(479-prevHuePoint-1,3,(~(hueImg.pixel(479-prevHuePoint-1,3))) | 0xff000000);
    hueImg.setPixel(479-prevHuePoint-1,4,(~(hueImg.pixel(479-prevHuePoint-1,4))) | 0xff000000);
    hueImg.setPixel(479-prevHuePoint-1,5,(~(hueImg.pixel(479-prevHuePoint-1,5))) | 0xff000000);
    hueImg.setPixel(479-prevHuePoint-1,6,(~(hueImg.pixel(479-prevHuePoint-1,6))) | 0xff000000);
    hueImg.setPixel(479-prevHuePoint-1,7,(~(hueImg.pixel(479-prevHuePoint-1,7))) | 0xff000000);
  }

  repaint_left = 479-prevHuePoint-1;
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

  canvas->fgHu = COLORDIV - hue;
  canvas->fgHu2 = ((double)(COLORDIV - hue))/(double)COLORDIV;

  if(huePoint == -1){
    huePoint = (int)(479*hue/COLORDIV);
  }
  prevHuePoint = 479 - huePoint;

  hueImg.setPixel(huePoint,2, (~(hueImg.pixel(huePoint,2))) | 0xff000000);
  hueImg.setPixel(huePoint,8, (~(hueImg.pixel(huePoint,8))) | 0xff000000);

  if(((int)huePoint)!=479){
	hueImg.setPixel(huePoint+1,2, (~(hueImg.pixel(huePoint+1,2))) | 0xff000000);
	hueImg.setPixel(huePoint+1,3, (~(hueImg.pixel(huePoint+1,3))) | 0xff000000);
	hueImg.setPixel(huePoint+1,4, (~(hueImg.pixel(huePoint+1,4))) | 0xff000000);
	hueImg.setPixel(huePoint+1,5, (~(hueImg.pixel(huePoint+1,5))) | 0xff000000);
	hueImg.setPixel(huePoint+1,6, (~(hueImg.pixel(huePoint+1,6))) | 0xff000000);
	hueImg.setPixel(huePoint+1,7, (~(hueImg.pixel(huePoint+1,7))) | 0xff000000);
  }

  if(((int)huePoint)!=0){
	hueImg.setPixel(huePoint-1,2, (~(hueImg.pixel(huePoint-1,2))) | 0xff000000);
	hueImg.setPixel(huePoint-1,3, (~(hueImg.pixel(huePoint-1,3))) | 0xff000000);
	hueImg.setPixel(huePoint-1,4, (~(hueImg.pixel(huePoint-1,4))) | 0xff000000);
	hueImg.setPixel(huePoint-1,5, (~(hueImg.pixel(huePoint-1,5))) | 0xff000000);
	hueImg.setPixel(huePoint-1,6, (~(hueImg.pixel(huePoint-1,6))) | 0xff000000);
	hueImg.setPixel(huePoint-1,7, (~(hueImg.pixel(huePoint-1,7))) | 0xff000000);
  }

}


void Hue::mousePressEvent( QMouseEvent *e )
{

  canvas->toolbar->reverseSBCursor();
  changeHueArea((e->x()*COLORDIV/479),e->x());

  canvas->toolbar->changeSBArea();
  canvas->toolbar->changeColorArea();

  //ツールバー画像の表示
  //     QPixmap huePix = QPixmap();
  //     huePix.convertFromImage(hueImg, 0);
  //     setBackgroundPixmap(huePix);

  //  QPixmap toolbarPix = QPixmap();
  //  toolbarPix.convertFromImage(canvas->toolbar->toolbarImg, 0);
  //  canvas->toolbar->setBackgroundPixmap(toolbarPix);

  repaint_left = e->x()-1;
  repaint_top = 0;
  repaint_width = 3;
  repaint_height = 20;


  if(repaint_left > 477){
    repaint_left = 477;
  }
  else if(repaint_left < 0){
    repaint_left = 0;
  }


  canvas->toolbar->repaintColor(repaint_left, repaint_top, repaint_width, repaint_height, true);

}


void Hue::mouseMoveEvent( QMouseEvent *e )
{

  canvas->toolbar->reverseSBCursor();
  changeHueArea((e->x()*COLORDIV/479),e->x());

  canvas->toolbar->changeSBArea();
  canvas->toolbar->changeColorArea();

  //ツールバー画像の表示
  //      QPixmap huePix = QPixmap();
  //      huePix.convertFromImage(hueImg, 0);
  //      setBackgroundPixmap(huePix);

  //  QPixmap toolbarPix = QPixmap();
  //  toolbarPix.convertFromImage(canvas->toolbar->toolbarImg, 0);
  //  canvas->toolbar->setBackgroundPixmap(toolbarPix);


  repaint_left = e->x()-1;
  repaint_top = 0;
  repaint_width = 3;
  repaint_height = 20;

  if(repaint_left > 477){
    repaint_left = 477;
  }
  else if(repaint_left < 0){
    repaint_left = 0;
  }

  canvas->toolbar->repaintColor(repaint_left, repaint_top, repaint_width, repaint_height, true);


}


void Hue::mouseReleaseEvent( QMouseEvent *e )
{

}

