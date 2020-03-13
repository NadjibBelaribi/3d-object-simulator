 #include "meshquad.h"
#include "matrices.h"

#define MAX 100000
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

    int i ,i1,i2,i3,i4;
     for(i = 0 ; i < quads.size()/4 ; i+=4)
     {
         i1 = quads.at(i) ;
         i2 = quads.at(i+1) ;
         i2 = quads.at(i+2) ;
         i2 = quads.at(i+3) ;

         if(i1 < i2)
         {
            edges.push_back(i1);
            edges.push_back(i2);
         }
         if(i2 < i3)
         {
            edges.push_back(i2);
            edges.push_back(i3);
         }
         if(i3 < i4)
         {
            edges.push_back(i3);
            edges.push_back(i4);
         }
         if(i4 < i1)
         {
            edges.push_back(i4);
            edges.push_back(i1);
         }
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
        int i1 = add_vertex(Vec3(-1,1,1));
        int i2 = add_vertex(Vec3(-1,-1,1));
        int i3 = add_vertex(Vec3(1,-1,1));
        int i4 = add_vertex(Vec3(1,1,1));
        int i5 = add_vertex(Vec3(-1,1,-1));
        int i6 = add_vertex(Vec3(-1,-1,-1));
        int i7 = add_vertex(Vec3(1,-1,-1));
        int i8 = add_vertex(Vec3(1,1,-1));

        // ajouter 6 faces (sens trigo)
        add_quad(i1,i2,i3,i4);
        add_quad(i4,i3,i7,i8);
        add_quad(i8,i7,i6,i5);
        add_quad(i5,i6,i2,i1);
        add_quad(i2,i6,i7,i3);
        add_quad(i1,i4,i8,i5);

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

    Vec3 n1 = glm::cross(B-A,C-B) ;
    Vec3 n2 = glm::cross(C-B,D-C) ;
    Vec3 n3 = glm::cross(D-C,A-D) ;
    Vec3 n4 = glm::cross(A-D,B-A) ;

    float som1 = P.x*n1.x + P.y*n1.y + P.z*n1.z ; //dot
    float som2 = P.x*n2.x + P.y*n2.y + P.z*n2.z ;
    float som3 = P.x*n3.x + P.y*n3.y + P.z*n3.z ;
    float som4 = P.x*n4.x + P.y*n4.y + P.z*n4.z ;

    return som1 > 0 && som2 >0 && som3 > 0  && som4 > 0 ;

}

bool MeshQuad::intersect_ray_quad(const Vec3& P, const Vec3& Dir, int q, Vec3& inter)
{
    // recuperation des indices de points
    // recuperation des points
    int i1,i2,i3,i4 ;
    Vec3 p1,p2,p3,p4 ;

    i1 = m_quad_indices.at(q) ;
    i2 = m_quad_indices.at(q+1) ;
    i3 = m_quad_indices.at(q+2) ;
    i4 = m_quad_indices.at(q+3) ;

    p1 = m_points.at(i1) ;
    p2 = m_points.at(i2) ;
    p3 = m_points.at(i3) ;
    p4 = m_points.at(i4) ;


    // calcul de l'equation du plan (N+d)

    Vec3 N = glm::cross(p2-p1,p3-p1) ;
    float d = -N.x*p4.x - N.y*p4.y - N.z*p4.z ;
    Vec3 D ;
    // calcul de l'intersection rayon plan
    // I = P + alpha*Dir est dans le plan => calcul de alpha

     float alpha = (d - glm::dot(N, P)) / glm::dot(P,Dir);
    // alpha => calcul de I

     inter = P + alpha * Dir ;
    // I dans le quad ?
    return is_points_in_quad(inter,p1,p2,p3,p4) ;
 }


int MeshQuad::intersected_closest(const Vec3& P, const Vec3& Dir)
{
    // on parcours tous les quads
    // on teste si il y a intersection avec le rayon
    // on garde le plus proche (de P)

    int  i,inter = -1 ;
    float min = MAX ;
    Vec3 inters = Vec3() ;

    for (i = 0  ; i < m_quad_indices.size()/4 ; i+=4)
    {
        if(intersect_ray_quad(P,Dir,i,inters))
        {
            if(glm::length(P-inters) < min)
            {
                inter = i ;
                min = glm::length(P-inters) ;
            }
        }
    }

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

    int i1,i2,i3,i4 ;
    Vec3 A,B,C,D ;

    i1 = m_quad_indices.at(q) ;
    i2 = m_quad_indices.at(q+1) ;
    i3 = m_quad_indices.at(q+2) ;
    i4 = m_quad_indices.at(q+3) ;

    A = m_points.at(i1) ;
    B = m_points.at(i2) ;
    C = m_points.at(i3) ;
    D = m_points.at(i4) ;

    // calcul de Z:N / X:AB -> Y

    Vec3 Z = glm::cross(B-A,C-A) ;
    Vec3 X = B - A ;
    Vec3 Y = Z / X  ;

    // calcul du centre
      C = normal_of(X,Y,Z) ;
    // calcul de la taille

      float t = glm::length(B-A) ;
    // calcul de la matrice

    Mat4 transfo = Mat4(Vec4(X,t),Vec4(Y,t),Vec4(Z,t),Vec4(C,t)) ;
    return transfo;
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

