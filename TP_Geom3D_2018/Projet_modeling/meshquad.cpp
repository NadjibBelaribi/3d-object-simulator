#include "meshquad.h"
#include "matrices.h"

using namespace std ;
void MeshQuad::clear()
{
    this->m_points.clear() ;
    this->m_quad_indices.clear() ;
    this->m_nb_ind_edges = 0 ;
}

int MeshQuad::add_vertex(const Vec3& P){

    this->m_points.push_back(P) ;
    return this->m_points.size() -1  ;
}


void MeshQuad::add_quad(int i1, int i2, int i3, int i4)
{
    this->m_quad_indices.push_back(i1) ;
    this->m_quad_indices.push_back(i2) ;
    this->m_quad_indices.push_back(i3) ;
    this->m_quad_indices.push_back(i4) ;

    this->m_nb_ind_edges  +=4 ;

}

void MeshQuad::convert_quads_to_tris(const std::vector<int>& quads, std::vector<int>& tris)
{
	tris.clear();
	tris.reserve(3*quads.size()/2);

	// Pour chaque quad on genere 2 triangles
	// Attention a repecter l'orientation des triangles

     int index = 0 ;

     for(size_t i = 0 ; i < quads.size()/4 ; i++)
     {
         tris.push_back(quads[index]) ;
         tris.push_back(quads[index+1]) ;
         tris.push_back(quads[index+2]) ;

         tris.push_back(quads[index]) ;
         tris.push_back(quads[index+2]) ;
         tris.push_back(quads[index+3]) ;

         index += 4 ;
     }



}

void MeshQuad::convert_quads_to_edges(const std::vector<int>& quads, std::vector<int>& edges)
{
	edges.clear();
	edges.reserve(quads.size()); // ( *2 /2 !)
	// Pour chaque quad on genere 4 aretes, 1 arete = 2 indices.
	// Mais chaque arete est commune a 2 quads voisins !
	// Comment n'avoir qu'une seule fois chaque arete ?


    int index = 0 ;
    unsigned int i ;

    for( i = 0 ; i < quads.size()/4 ; i++)
    {
        if (i == 0 )
        {
              edges.push_back(quads[index]) ;
              edges.push_back(quads[index+1]) ;
        }
        edges.push_back(quads[index+1]) ;
        edges.push_back(quads[index+2]) ;
        edges.push_back(quads[index+2]) ;
        edges.push_back(quads[index+3]) ;
        edges.push_back(quads[index+3]) ;
        edges.push_back(quads[index]) ;


        index += 4 ;
    }

}


void MeshQuad::bounding_sphere(Vec3& C, float& R)
{
	// C=
	// R=
}


void MeshQuad::create_cube()
{
    clear();

    // ajouter 8 sommets (-1 +1)
    Vec3 p0, p1, p2, p3, p4, p5, p6, p7, p8;

        p0= Vec3(-0.5,-0.5,+0.5) ;  // P0
        p1= Vec3(0.5,-0.5,+0.5);  // P1
        p2= Vec3(0.5,+0.5,+0.5);  // P2
        p3= Vec3(-0.5,+0.5,+0.5);  // P3
        p4= Vec3(-0.5,-0.5,-0.5);  // P4
        p5= Vec3(0.5,-0.5,-0.5);  // P5
        p6= Vec3(0.5,+0.5,-0.5);  // P6
        p7= Vec3(-0.5,+0.5,-0.5);   // P7


    int i0, i1, i2, i3, i4, i5, i6, i7;

    i0 = add_vertex(p0);
    i1 = add_vertex(p1);
    i2 = add_vertex(p2);
    i3 = add_vertex(p3);
    i4 = add_vertex(p4);
    i5 = add_vertex(p5);
    i6 = add_vertex(p6);
    i7 = add_vertex(p7);

    // ajouter 6 faces (sens trigo)
    add_quad(i1, i2, i3, i0);
    add_quad(i3, i2, i6, i7);
    add_quad(i1, i2, i6, i5);
    add_quad(i0, i3, i7, i4);
    add_quad(i5, i6, i7, i4);
    add_quad(i1, i5, i4, i0);

    gl_update();
}

Vec3 MeshQuad::normal_of(const Vec3& A, const Vec3& B, const Vec3& C)
{
	// Attention a l'ordre des points !
	// le produit vectoriel n'est pas commutatif U ^ V = - V ^ U
	// ne pas oublier de normaliser le resultat.

    Vec3 AB = Vec3(B[0]-A[0],B[1]-A[1],B[2]-A[2]) ;
    Vec3 BC = Vec3(C[0]-B[0],C[1]-B[1],C[2]-B[2]) ;

    float x = AB[1]*BC[2] - AB[1]*BC[2] ;
    float y = AB[1]*BC[2] - AB[1]*BC[2] ;
    float z = AB[1]*BC[2] - AB[1]*BC[2];

    Vec3 N = Vec3(x,y,z) ;
    float norme = sqrt(x*x+y*y+z*z) ;

    return N/norme;
}


float MeshQuad::calcul_aire(Vec3 p1,Vec3 p2,Vec3 p3,Vec3 p4)
{
    float seg1=0.,seg2=0. ;

    seg1 = sqrt(pow(p2[0]-p1[0],2)+pow(p2[1]-p1[1],2)+pow(p2[2]-p1[2],2)) ;
    seg2 = sqrt(pow(p3[0]-p2[0],2)+pow(p3[1]-p2[1],2)+pow(p3[2]-p2[2],2)) ;

    return seg1 * seg2 ;

}

bool MeshQuad::is_points_in_quad(const Vec3& P, const Vec3& A, const Vec3& B, const Vec3& C, const Vec3& D)
{
	// On sait que P est dans le plan du quad.

	// P est-il au dessus des 4 plans contenant chacun la normale au quad et une arete AB/BC/CD/DA ?
	// si oui il est dans le quad

    return true;
}

bool MeshQuad::intersect_ray_quad(const Vec3& P, const Vec3& Dir, int q, Vec3& inter)
{
	// recuperation des indices de points
	// recuperation des points

	// calcul de l'equation du plan (N+d)

    // calcul de l'intersection rayon plan
	// I = P + alpha*Dir est dans le plan => calcul de alpha

	// alpha => calcul de I

	// I dans le quad ?

    return false;
}


int MeshQuad::intersected_closest(const Vec3& P, const Vec3& Dir)
{
	// on parcours tous les quads
	// on teste si il y a intersection avec le rayon
	// on garde le plus proche (de P)

	int inter = -1;

	return inter;
}


Mat4 MeshQuad::local_frame(int q)
{
	// Repere locale = Matrice de transfo avec
	// les trois premieres colones: X,Y,Z locaux
	// la derniere colonne l'origine du repere

	// ici Z = N et X = AB
	// Origine le centre de la face
	// longueur des axes : [AB]/2

	// recuperation des indices de points
	// recuperation des points

	// calcul de Z:N / X:AB -> Y

	// calcul du centre

	// calcul de la taille

	// calcul de la matrice

    return Mat4();
}

void MeshQuad::extrude_quad(int q)
{
	// recuperation des indices de points

	// recuperation des points

	// calcul de la normale

	// calcul de la hauteur

	// calcul et ajout des 4 nouveaux points

	// on remplace le quad initial par le quad du dessu

	// on ajoute les 4 quads des cotes

   gl_update();
}

void MeshQuad::transfo_quad(int q, const glm::mat4& tr)
{
	// recuperation des indices de points
	// recuperation des (references de) points

	// generation de la matrice de transfo globale:
	// indice utilisation de glm::inverse() et de local_frame

	// Application au 4 points du quad
}

void MeshQuad::decale_quad(int q, float d)
{

}

void MeshQuad::shrink_quad(int q, float s)
{

}

void MeshQuad::tourne_quad(int q, float a)
{

}





MeshQuad::MeshQuad():
	m_nb_ind_edges(0)
{}


void MeshQuad::gl_init()
{
	m_shader_flat = new ShaderProgramFlat();
	m_shader_color = new ShaderProgramColor();

	//VBO
	glGenBuffers(1, &m_vbo);

	//VAO
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(m_shader_flat->idOfVertexAttribute);
	glVertexAttribPointer(m_shader_flat->idOfVertexAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &m_vao2);
	glBindVertexArray(m_vao2);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(m_shader_color->idOfVertexAttribute);
	glVertexAttribPointer(m_shader_color->idOfVertexAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);

	//EBO indices
	glGenBuffers(1, &m_ebo);
	glGenBuffers(1, &m_ebo2);
}

void MeshQuad::gl_update()
{
	//VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * m_points.size() * sizeof(GLfloat), &(m_points[0][0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	std::vector<int> tri_indices;
	convert_quads_to_tris(m_quad_indices,tri_indices);

	//EBO indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,tri_indices.size() * sizeof(int), &(tri_indices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	std::vector<int> edge_indices;
	convert_quads_to_edges(m_quad_indices,edge_indices);
	m_nb_ind_edges = edge_indices.size();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,m_nb_ind_edges * sizeof(int), &(edge_indices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}



void MeshQuad::set_matrices(const Mat4& view, const Mat4& projection)
{
	viewMatrix = view;
	projectionMatrix = projection;
}

void MeshQuad::draw(const Vec3& color)
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);

	m_shader_flat->startUseProgram();
	m_shader_flat->sendViewMatrix(viewMatrix);
	m_shader_flat->sendProjectionMatrix(projectionMatrix);
	glUniform3fv(m_shader_flat->idOfColorUniform, 1, glm::value_ptr(color));
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ebo);
	glDrawElements(GL_TRIANGLES, 3*m_quad_indices.size()/2,GL_UNSIGNED_INT,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	glBindVertexArray(0);
	m_shader_flat->stopUseProgram();

	glDisable(GL_POLYGON_OFFSET_FILL);

	m_shader_color->startUseProgram();
	m_shader_color->sendViewMatrix(viewMatrix);
	m_shader_color->sendProjectionMatrix(projectionMatrix);
	glUniform3f(m_shader_color->idOfColorUniform, 0.0f,0.0f,0.0f);
	glBindVertexArray(m_vao2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ebo2);
	glDrawElements(GL_LINES, m_nb_ind_edges,GL_UNSIGNED_INT,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	glBindVertexArray(0);
	m_shader_color->stopUseProgram();
	glDisable(GL_CULL_FACE);
}

