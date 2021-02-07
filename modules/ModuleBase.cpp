#include <QGridLayout>

#include "ModuleBase.h"

namespace modules {

QGridLayout &ModuleBase::getOrCreateInnerGrid()
{
  if (innerGrid)
    return *innerGrid;
  
  QWidget &moduleWidget = getWidget();
  innerGrid = new QGridLayout(&moduleWidget);
  innerGrid->setMargin(2);
  return *innerGrid;
}

ModuleAnnotation::ModuleAnnotation(ModuleBase &module, Qt::Alignment align,
                                   int row, int col)
    : QLabel(&module.getWidget())
{
  setForegroundRole(QPalette::ColorRole::Dark);
  setAlignment(align);
  
  QFont labelFont(font());
  labelFont.setPointSize(10);
  setFont(labelFont);
  setWordWrap(true);

  QGridLayout &innerGrid = module.getOrCreateInnerGrid();
  innerGrid.addWidget(this, row, col, align);
}
}