#include "autogeneratetransitiondialog.h"
#include "ui_autogeneratetransitiondialog.h"

enum TransitionFlag {
    TF_LEFT         = 1,
    TF_TOP_LEFT     = 2,
    TF_TOP          = 4,
    TF_TOP_RIGHT    = 8,
    TF_RIGHT        = 16,
    TF_BOTTOM_RIGHT = 32,
    TF_BOTTOM       = 64,
    TF_BOTTOM_LEFT  = 128,

    TF_COMPLETE_LEFT = TF_LEFT | TF_TOP_LEFT | TF_BOTTOM_LEFT,
    TF_COMPLETE_RIGHT = TF_RIGHT | TF_TOP_RIGHT | TF_BOTTOM_RIGHT,
    TF_COMPLETE_TOP = TF_TOP_LEFT | TF_TOP | TF_TOP_RIGHT,
    TF_COMPLETE_BOTTOM = TF_BOTTOM_LEFT | TF_BOTTOM | TF_BOTTOM_RIGHT,
    TF_COMPLETE = TF_COMPLETE_LEFT | TF_COMPLETE_TOP | TF_COMPLETE_RIGHT | TF_COMPLETE_BOTTOM,
    TF_BOTTOM_LEFT_CORNER = TF_LEFT | TF_BOTTOM_LEFT | TF_BOTTOM,
    TF_BOTTOM_RIGHT_CORNER = TF_RIGHT | TF_BOTTOM_RIGHT | TF_BOTTOM,
    TF_TOP_LEFT_CORNER = TF_LEFT | TF_TOP_LEFT | TF_TOP,
    TF_TOP_RIGHT_CORNER = TF_RIGHT | TF_TOP_RIGHT | TF_TOP,

};

AutoGenerateTransitionDialog::AutoGenerateTransitionDialog(MapPtr map, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoGenerateTransitionDialog),
    mMap(map)
{
    ui->setupUi(this);
}

AutoGenerateTransitionDialog::~AutoGenerateTransitionDialog()
{
    delete ui;
}

void AutoGenerateTransitionDialog::accept() {
    QDialog::accept();

    generate();
}

void AutoGenerateTransitionDialog::generate() {
    grid2d<unsigned int> &tiles(mMap->getTiles());
    grid2d<unsigned int> flagsGrid(tiles.getSizeX(), tiles.getSizeY(), 0);
    // scan all tiles
    for (unsigned int x = 1; x < mMap->getTiles().getSizeX() - 1; x++) {
        for (unsigned int y = 1; y < mMap->getTiles().getSizeY() - 1; y++) {
            // only change default bricks
            if (mMap->getTiles()(x, y) != 1) {continue;}
            unsigned int flags = 0;
            if (tiles(x - 1, y) == 2) {flags |= TF_LEFT;}
            if (tiles(x - 1, y + 1) == 2) {flags |= TF_TOP_LEFT;}
            if (tiles(x, y + 1) == 2) {flags |= TF_TOP;}
            if (tiles(x + 1, y + 1) == 2) {flags |= TF_TOP_RIGHT;}
            if (tiles(x + 1, y) == 2) {flags |= TF_RIGHT;}
            if (tiles(x + 1, y - 1) == 2) {flags |= TF_BOTTOM_RIGHT;}
            if (tiles(x, y - 1) == 2) {flags |= TF_BOTTOM;}
            if (tiles(x - 1, y - 1) == 2) {flags |= TF_BOTTOM_LEFT;}

            flagsGrid(x, y) = flags;
        }
    }

    // place
    for (unsigned int x = 1; x < mMap->getTiles().getSizeX() - 1; x++) {
        for (unsigned int y = 1; y < mMap->getTiles().getSizeY() - 1; y++) {
            unsigned int flags = flagsGrid(x, y);
            if (flags == 0) {continue;}

            if (flags == TF_LEFT || flags == TF_COMPLETE_LEFT
                    || flags == TF_LEFT + TF_TOP_LEFT || flags == TF_LEFT + TF_BOTTOM_LEFT) {
                tiles(x, y) = 3;
            }
            else if (flags == TF_RIGHT
                     || flags == TF_COMPLETE_RIGHT
                     || flags == TF_RIGHT + TF_TOP_RIGHT
                     || flags == TF_RIGHT + TF_BOTTOM_RIGHT) {
                tiles(x, y) = 4;
            }
            else if (flags == TF_COMPLETE_TOP
                     || flags == TF_TOP
                     || flags == (TF_TOP | TF_TOP_RIGHT)
                     || flags == (TF_TOP | TF_TOP_LEFT)) {
                tiles(x, y) = 5;
            }
            else if (flags == TF_COMPLETE_BOTTOM
                     || flags == (TF_BOTTOM | TF_BOTTOM_LEFT)
                     || flags == (TF_BOTTOM | TF_BOTTOM_RIGHT)
                     || flags == (TF_BOTTOM)) {
                tiles(x, y) = 6;
            }
            else if ((flags & (TF_TOP | TF_BOTTOM)) == (TF_TOP | TF_BOTTOM)) {
                if ((flags & (TF_LEFT | TF_RIGHT)) == (TF_LEFT | TF_RIGHT)) {
                    tiles(x, y) = 16;
                }
                else if (flags & TF_LEFT) {
                    tiles(x, y) = 18;
                }
                else if (flags & TF_RIGHT) {
                    tiles(x, y) = 19;
                }
                else {
                    tiles(x, y) = 17;
                }
            }
            else if (flags == (TF_COMPLETE_TOP | TF_COMPLETE_BOTTOM | TF_COMPLETE_LEFT)) {
                tiles(x, y) = 18;
            }
            else if (flags == (TF_COMPLETE_TOP | TF_COMPLETE_BOTTOM | TF_COMPLETE_RIGHT)) {
                tiles(x, y) = 19;
            }
            else if (flags == TF_COMPLETE) {
                tiles(x, y) = 16;
            }
            else if (flags == TF_BOTTOM_LEFT_CORNER
                     || flags == (TF_BOTTOM_LEFT_CORNER | TF_TOP_LEFT)
                     || flags == (TF_BOTTOM_LEFT_CORNER | TF_TOP_LEFT | TF_BOTTOM_RIGHT)
                     || flags == (TF_BOTTOM_LEFT_CORNER | TF_BOTTOM_RIGHT)) {
                tiles(x, y) = 13;
            }
            else if (flags == TF_BOTTOM_RIGHT_CORNER
                     || flags == (TF_BOTTOM_RIGHT_CORNER | TF_TOP_RIGHT)
                     || flags == (TF_BOTTOM_RIGHT_CORNER | TF_TOP_RIGHT | TF_BOTTOM_LEFT)
                     || flags == (TF_BOTTOM_RIGHT_CORNER | TF_BOTTOM_LEFT)) {
                tiles(x, y) = 14;
            }
            else if (flags == TF_BOTTOM_LEFT) {
                tiles(x, y) = 11;
            }
            else if (flags == TF_BOTTOM_RIGHT) {
                tiles(x, y) = 12;
            }
            else if (flags == TF_TOP_LEFT_CORNER
                     || flags == (TF_TOP_LEFT_CORNER | TF_TOP_RIGHT)
                     || flags == (TF_TOP_LEFT_CORNER | TF_TOP_RIGHT | TF_BOTTOM_LEFT)
                     || flags == (TF_TOP_LEFT_CORNER | TF_BOTTOM_LEFT)) {
                tiles(x, y) = 7;
            }
            else if (flags == TF_TOP_RIGHT_CORNER
                     || flags == (TF_TOP_RIGHT_CORNER | TF_TOP_LEFT)
                     || flags == (TF_TOP_RIGHT_CORNER | TF_TOP_LEFT | TF_BOTTOM_RIGHT)
                     || flags == (TF_TOP_RIGHT_CORNER | TF_BOTTOM_RIGHT) ) {
                tiles(x, y) = 8;
            }
            else if (flags == (TF_BOTTOM_LEFT | TF_BOTTOM_RIGHT)) {
                tiles(x, y) = 20;
            }
            else if (flags == (TF_COMPLETE_BOTTOM | TF_LEFT | TF_RIGHT)
                     || flags == (TF_COMPLETE_BOTTOM | TF_COMPLETE_LEFT | TF_RIGHT)
                     || flags == (TF_COMPLETE_BOTTOM | TF_LEFT | TF_COMPLETE_RIGHT)
                     || flags == (TF_COMPLETE_BOTTOM | TF_COMPLETE_LEFT | TF_COMPLETE_RIGHT)) {
                tiles(x, y) = 15;
            }
            else if ((flags & (TF_LEFT | TF_RIGHT)) == (TF_LEFT | TF_RIGHT)) {
                if (flags & TF_TOP) {
                    tiles(x, y) = 9;
                }
                else {
                    tiles(x, y) = 10;
                }
            }
        }
    }
}
