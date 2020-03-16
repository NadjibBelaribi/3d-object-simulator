
#include <QApplication>
#include <QGLViewer/simple_viewer.h>
#include <matrices.h>
#include <primitives.h>
#include <meshquad.h>

const Vec3 ROUGE   = {1,0,0};
const Vec3 VERT    = {0,1,0};
const Vec3 BLEU    = {0,0,1};
const Vec3 JAUNE   = {1,1,0};
const Vec3 CYAN    = {0,1,1};
const Vec3 MAGENTA = {1,0,1};
const Vec3 BLANC   = {1,1,1};
const Vec3 GRIS    = {0.5,0.5,0.5};
const Vec3 NOIR    = {0,0,0};



void draw_repere(const Primitives& prim, const Mat4& tr)
{
     prim.draw_sphere(tr, BLANC);
    prim.draw_cylinder(tr*translate(1,0,0)* scale(2,0.5,0.5) * rotateY(90) , ROUGE); // X
    prim.draw_cylinder(tr*translate(0,1,0)*  scale(0.5,2,0.5) * rotateX(90)  , VERT); //Y
    prim.draw_cylinder(tr*translate(0,0,1) * scale(0.5,0.5,2) , BLEU); //Z

    prim.draw_cone(tr*translate(2,0,0)* rotateY(90), ROUGE);
    prim.draw_cone(tr*translate(0,2,0)* rotateX(-90), VERT);
    prim.draw_cone(tr*translate(0,0,2), BLEU);
}


void star(MeshQuad& m)
{
	m.create_cube();
    float rot = 0 ;
    for (int i = 0 ; i < 10 ; i++)
        for (int j = 0 ; j < 6 ; j++){
             rot = 20 + 2*i ;
             m.extrude_quad(j*4) ;
             m.shrink_quad(j*4,0.8) ;
             m.tourne_quad(j*4,rot) ;

        }

}

void spirale(MeshQuad& m)
{

}




int main(int argc, char *argv[])
{
	Primitives prim;
	int selected_quad = -1;
	glm::mat4 selected_frame;
	MeshQuad mesh;

	// init du viewer
	QApplication a(argc, argv);
	SimpleViewer::init_gl();
	SimpleViewer viewer({0.1,0.1,0.1},5);

	// GL init
	viewer.f_init = [&] ()
	{
		prim.gl_init();
		mesh.gl_init();
	};

	// drawing
	viewer.f_draw = [&] ()
	{
		mesh.set_matrices(viewer.getCurrentModelViewMatrix(),viewer.getCurrentProjectionMatrix());
		prim.set_matrices(viewer.getCurrentModelViewMatrix(),viewer.getCurrentProjectionMatrix());

        mesh.draw(CYAN);

        if (selected_quad>=0)
        {
            //draw_repere(prim,selected_frame);
            //std::cout << "selected is N° "<< selected_quad << std::endl;
         }
	};

    Mat4 transfo  =translate(0,1,0) ;
	// to do when key pressed
	viewer.f_keyPress = [&] (int key, Qt::KeyboardModifiers mod)
	{
		switch(key)
		{
			case Qt::Key_C:
				if (!(mod & Qt::ControlModifier))
					mesh.create_cube();
				break;

			// e extrusion
            case Qt::Key_E:
            if (!(mod & Qt::ControlModifier))
                if (selected_quad >=0 ) mesh.extrude_quad(selected_quad) ;
            break;

            // +/- decale
             case Qt::Key_Plus:
                 if (!(mod & Qt::ControlModifier))
                 if (selected_quad >=0 ) mesh.decale_quad(selected_quad,1) ;

            break ;

             case Qt::Key_Minus:
            if (!(mod & Qt::ControlModifier))
            if (selected_quad
                    >=0 ) mesh.decale_quad(selected_quad,-1) ;
             break ;
			// z/Z shrink
             case Qt::Key_Z:
                 if (!(mod & Qt::ControlModifier))
                 if (selected_quad >=0 ) mesh.shrink_quad(selected_quad,0.5) ;
             break ;
			// t/T tourne
            case Qt::Key_T:
                 if (!(mod & Qt::ControlModifier))
                 if (selected_quad >=0 ) mesh.tourne_quad(selected_quad,1)  ;
               break ;
			// Attention au cas m_selected_quad == -1

			// generation d'objet
			case Qt::Key_S:
				star(mesh);
				break;

        case Qt::Key_P:
             spirale(mesh) ;
            break;
             // ....


			default:
				break;
		}

		Vec3 sc;
		float r;
		mesh.bounding_sphere(sc,r);
		viewer.setSceneCenter(qglviewer::Vec(sc[0],sc[1],sc[2]));
		viewer.setSceneRadius(r);
		viewer.camera()->centerScene();
		viewer.update();
	};

	// to do when mouse clicked (P + Dir = demi-droite (en espace objet) orthogonale à l'écran passant par le point cliqué
	viewer.f_mousePress3D = [&] (Qt::MouseButton /*b*/, const glm::vec3& P, const glm::vec3& Dir)
	{
		selected_quad = mesh.intersected_closest(P,Dir);
		if (selected_quad>=0)
			selected_frame = mesh.local_frame(selected_quad);
        //std::cout << selected_quad << std::endl;
	};

	viewer.clearShortcuts();
	viewer.setShortcut(QGLViewer::EXIT_VIEWER,Qt::Key_Escape);
	viewer.show();
	return a.exec();
}
