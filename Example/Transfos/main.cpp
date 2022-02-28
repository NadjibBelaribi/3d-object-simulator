
#include <QApplication>
#include <QGLViewer/simple_viewer.h>
#include <matrices.h>
#include <primitives.h>

using namespace std ;

const Vec3 ROUGE   = {1,0,0};
const Vec3 VERT    = {0,1,0};
const Vec3 BLEU    = {0,0,1};
const Vec3 JAUNE   = {1,1,0};
const Vec3 CYAN    = {0,1,1};
const Vec3 MAGENTA = {1,0,1};
const Vec3 BLANC   = {1,1,1};
const Vec3 GRIS    = {0.5,0.5,0.5};
const Vec3 NOIR    = {0,0,0};

/// primitives(sphere/cube/cylindre/cone)
Primitives prim;

/// compteur (incremente dans animate)
int compteur = 0;





void draw_repere(const Mat4& tr)
{


    prim.draw_sphere(tr, BLANC);
    prim.draw_cylinder(tr*translate(1,0,0)* scale(2,0.5,0.5) * rotateY(90) , ROUGE);
    prim.draw_cylinder(tr*translate(0,1,0)*  scale(0.5,2,0.5) * rotateX(90)  , VERT);
    prim.draw_cylinder(tr*translate(0,0,1) * scale(0.5,0.5,2) , BLEU);

    prim.draw_cone(tr*translate(2,0,0)* rotateY(90), ROUGE);
    prim.draw_cone(tr*translate(0,2,0)* rotateX(-90), VERT);
    prim.draw_cone(tr*translate(0,0,2), BLEU);

   }



void articulations(const Mat4& transfo)
{
   //prim.draw_sphere(transfo,BLANC) ;

      prim.draw_cube(transfo*scale(1,0.5,0.5),BLEU);
     prim.draw_sphere(transfo* translate(1,0,0),BLANC) ;

     prim.draw_cube(transfo* translate(2,0,0)*scale(1,0.5,0.5),VERT) ;
    prim.draw_sphere(transfo* translate(3,0,0),BLANC) ;

    prim.draw_cube(transfo* translate(4,0,0)*scale(1,0.5,0.5),ROUGE);
    prim.draw_sphere(transfo* translate(5,0,0),BLANC) ;

}



int main(int argc, char *argv[])
{

    Mat4 m1 = translate(0,1,2) ;
    Vec3 v1  (2,2,2) ;

    /* Q1
     * Vec4 v3 ,v2 = Vec4(v1,1) ;
    v3 = m1 * v2 ;
    cout << m1 << endl ;
    cout << v1 << endl  ;
    cout << Vec3(v3) << endl ;*/

    Mat4 m2 = Mat4();

    /* Q2

    cout <<"rotation + translation " << m1 * m2 << endl  ;
    cout << " translation  + rotation " <<m2 * m1 << endl ;*/



	// init du viewer
	QApplication a(argc, argv);
	SimpleViewer::init_gl();
	// ouvre une fenetre QGLViewer avec fond noir et une scene de rayon 10
	SimpleViewer viewer(NOIR,10);

	// GL init
	viewer.f_init = [&] ()
	{
		prim.gl_init();
	};

	// drawing
	viewer.f_draw = [&] ()
	{
		prim.set_matrices(viewer.getCurrentModelViewMatrix(), viewer.getCurrentProjectionMatrix());

       /* prim.draw_sphere(Mat4(), BLANC);
        prim.draw_cube(rotate(translate(2,0,0), ROUGE);
		prim.draw_cone(translate(0,2,0), VERT);
        prim.draw_cylinder(translate(0,0,2), BLEU);*/


         draw_repere(m2) ;
        /* for (int i = 0 ; i< 10 ; i++)
         {
             draw_repere(m2*rotateY(45*i)*translate(10,0,0)*scale(0.5)*m2*rotateY(-90));
         }
        prim.draw_cube(translate(0,1,0)*scale(5,1,10),BLANC);

       for (int i = 1 ; i< 4 ; i++){
        articulations(translate(0,0,2*i)* rotateX(130));
        }*/
};


	// to do when key pressed
    viewer.f_keyPress = [&] (int key, Qt::KeyboardModifiers /*mod*/)
	{
		switch(key)
		{
			case Qt::Key_A: // touche 'a'
				if (viewer.animationIsStarted())
					viewer.stopAnimation();
				else
					viewer.startAnimation();
				break;
			default:
				break;
		}
	};

	// to do every 50th of sec
	viewer.f_animate = [&] ()
	{
      /*  m2 = m2*rotateZ(-10) ;
        m2 = m2 *rotateX(90) ;*/
		compteur++;
		std::cout << "animate :" << compteur<< std::endl;
	};


	viewer.show();
	return a.exec();
}
