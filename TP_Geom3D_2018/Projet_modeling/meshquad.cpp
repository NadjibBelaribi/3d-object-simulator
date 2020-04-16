#include "meshquad.h"
#include "matrices.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <omp.h>

using namespace std;

void MeshQuad::clear()
{
    m_points.clear();
    m_quad_indices.clear();
    m_nb_ind_edges = 0;
}

int MeshQuad::add_vertex(const Vec3& P)
{
    m_points.push_back(P);
    return m_points.size() - 1;
}


void MeshQuad::add_quad(int i1, int i2, int i3, int i4)
{
    m_quad_indices.push_back(i1);
    m_quad_indices.push_back(i2);
    m_quad_indices.push_back(i3);
    m_quad_indices.push_back(i4);

    m_nb_ind_edges += 4;
}

void MeshQuad::convert_quads_to_tris(const std::vector<int>& quads, std::vector<int>& tris)
{
    tris.clear();
    tris.reserve(3*quads.size()/2);

    // Pour chaque quad on genere 2 triangles
    // Attention a repecter l'orientation des triangles

    #pragma omp parallel for
    for (size_t i=0; i< quads.size(); i+=4) {
        int v1 = quads.at(i);
        int v2 = quads.at(i+1);
        int v3 = quads.at(i+2);
        int v4 = quads.at(i+3);

        tris.push_back(v1);
        tris.push_back(v2);
        tris.push_back(v3);
        tris.push_back(v3);
        tris.push_back(v4);
        tris.push_back(v1);
     }

}

void MeshQuad::convert_quads_to_edges(const std::vector<int>& quads, std::vector<int>& edges)
{
    edges.clear();
    edges.reserve(quads.size()); // ( *2 /2 !)

    // Pour chaque quad on genere 4 aretes, 1 arete = 2 indices.
    // Mais chaque arete est commune a 2 quads voisins !
    // Comment n'avoir qu'une seule fois chaque arete ?

    size_t i, nb_quads = quads.size();

    #pragma omp parallel for
    for (i = 0; i < nb_quads; i+=4) {
        int i1 = quads.at(i);
        int i2 = quads.at(i+1);
        int i3 = quads.at(i+2);
        int i4 = quads.at(i+3);

        if (i1 < i2) {
           edges.push_back(i1);
           edges.push_back(i2);
        }
        if (i2 < i3) {
           edges.push_back(i2);
           edges.push_back(i3);
        }
        if (i3 < i4) {
           edges.push_back(i3);
           edges.push_back(i4);
        }
        if (i4 < i1) {
           edges.push_back(i4);
           edges.push_back(i1);
        }
      }
}


void MeshQuad::bounding_sphere(Vec3& C, float& R)
{
      size_t i , nb_points = m_points.size();

      #pragma omp parallel for reduction(+:C)
      for (i = 0; i < nb_points; i++) {
          C += m_points.at(i);
      }
      C /= nb_points;

      float length ,max = glm::length(m_points.at(0) - C);

      for (i = 1; i < nb_points; i++) {
          length = glm::length(m_points.at(i) - C);
          if (length > max) {
              max = length;
          }
      }
      R = max;
}


void MeshQuad::create_cube()
{
    clear();
    float s = 0.5 ;

    // ajouter 8 sommets (-1 +1)
    int i1 = add_vertex(Vec3(-s,s,s));
    int i2 = add_vertex(Vec3(-s,-s,s));
    int i3 = add_vertex(Vec3(s,-s,s));
    int i4 = add_vertex(Vec3(s,s,s));
    int i5 = add_vertex(Vec3(-s,s,-s));
    int i6 = add_vertex(Vec3(-s,-s,-s));
    int i7 = add_vertex(Vec3(s,-s,-s));
    int i8 = add_vertex(Vec3(s,s,-s));

    // ajouter 6 faces (sens trigo)
    add_quad(i5,i6,i2,i1);
    add_quad(i2,i6,i7,i3);
    add_quad(i1,i4,i8,i5);
    add_quad(i1,i2,i3,i4);
    add_quad(i4,i3,i7,i8);
    add_quad(i8,i7,i6,i5);

    gl_update();
}

Vec3 MeshQuad::normal_of(const Vec3& A, const Vec3& B, const Vec3& C)
{
    // Attention a l'ordre des points !
    // le produit vectoriel n'est pas commutatif U ^ V = - V ^ U
    // ne pas oublier de normaliser le resultat.

    Vec3 normal = glm::cross(B - A, C - A);
    return glm::normalize(normal);
 }

float MeshQuad::quad_aire(int q)
{
    int v1, v2, v3, v4;
    v1 = m_quad_indices.at(q);
    v2 = m_quad_indices.at(q+1);
    v3 = m_quad_indices.at(q+2);
    v4 = m_quad_indices.at(q+3);

    Vec3 A, B, C, D;
    A = m_points.at(v1);
    B = m_points.at(v2);
    C = m_points.at(v3);
    D = m_points.at(v4);

    float a1 , a2;

    // Aire triangle ABC
    a1 = glm::length(glm::cross(B - A, C - A))/2;

    // Aire triangle ACD
    a2 = glm::length(glm::cross(C - A, D - A))/2;

    return a1 + a2;
}


bool MeshQuad::is_points_in_quad(const Vec3& P, const Vec3& A, const Vec3& B, const Vec3& C, const Vec3& D)
{
    // On sait que P est dans le plan du quad.
    // P est-il au dessus des 4 plans contenant chacun la normale au quad et une arete AB/BC/CD/DA ?
    // si oui il est dans le quad

    Vec3 n1, n2, n3, n4, n, AB, BC, CD, DA;
    n = normal_of(A, B, C);
    AB = B - A;
    BC = C - B;
    CD = D - C;
    DA = A - D;

    // AB
    n1 = glm::cross(n, AB);
    if (glm::dot(n1, P) - glm::dot(n1, A)  < 0)
        return false;

    // BC
    n2 = glm::cross(n, BC);
    if ( glm::dot(n2, P) - glm::dot(n2, B) < 0)
        return false;

    // CD
    n3 = glm::cross(n, CD);
    if (glm::dot(n3, P) - glm::dot(n3, C) < 0)
        return false;

    // DA
    n4 = glm::cross(n, DA);
    if (glm::dot(n4, P) - glm::dot(n4, D)< 0)
        return false;

    return true;
}

bool MeshQuad::intersect_ray_quad(const Vec3& P, const Vec3& Dir, int q, Vec3& inter)
{
    // recuperation des indices de points
    int i1, i2, i3, i4;
    i1 = m_quad_indices.at(q);
    i2 = m_quad_indices.at(q+1);
    i3 = m_quad_indices.at(q+2);
    i4 = m_quad_indices.at(q+3);

    // recuperation des points
    Vec3 A, B, C, D;
    A = m_points.at(i1);
    B = m_points.at(i2);
    C = m_points.at(i3);
    D = m_points.at(i4);

    // calcul de l'equation du plan (N+d)
    Vec3 n = normal_of(A, B, C);
    float d = glm::dot(n, A);

    // calcul de l'intersection rayon plan
    float alpha = (d - glm::dot(n, P)) / glm::dot(n, Dir);

    // I = P + alpha*Dir est dans le plan => calcul de alpha
    // alpha => calcul de I
    inter = P + alpha*Dir;

    // I dans le quad ?
    return is_points_in_quad(inter, A, B, C, D);
}


int MeshQuad::intersected_closest(const Vec3& P, const Vec3& Dir)
{
    // on parcours tous les quads
    // on teste si il y a intersection avec le rayon
    // on garde le plus proche (de P)


    int  i,inter = -1 ;
    float temp , min = FLT_MAX ;
    Vec3 inters = Vec3() ;

    int nb_quads  = m_quad_indices.size() ;
    #pragma omp parallel for
    for (i = 0  ; i < nb_quads ; i+=4)
    {
        if(intersect_ray_quad(P,Dir,i,inters))
        {
            temp = glm::length(P - inters) ;
            if( temp  < min)
            {
                inter = i ;
                min = temp ;
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
    int i1, i2, i3, i4;
    i1 = m_quad_indices.at(q);
    i2 = m_quad_indices.at(q+1);
    i3 = m_quad_indices.at(q+2);
    i4 = m_quad_indices.at(q+3);

    // recuperation des points
    Vec3 A, B, C, D, AB;
    A = m_points.at(i1);
    B = m_points.at(i2);
    C = m_points.at(i3);
    D = m_points.at(i4);
    AB = B - A;

    // calcul de Z:N / X:AB -> Y
    Vec3 X, Y, Z, n;

     n = normal_of(A, B, C) + normal_of(A, C, D);
     n /= 2 ;

    Z = glm::normalize(n);
    X = glm::normalize(glm::cross(AB, Z));
    Y = glm::normalize(glm::cross(X, Z));

    // calcul du centre
    Vec3 center = (A + B + C + D);
    center /= 4 ;

    // calcul de la taille
    float taille = glm::length(AB)/2;

    // calcul de la matrice
    Mat4 result = Mat4(Vec4(X, 0), Vec4(Y, 0), Vec4(Z, 0), Vec4(center, 1));
    result *= scale(taille);

    return result;

}

void MeshQuad::extrude_quad(int q)
{
    // recuperation des indices de points
    int i1, i2, i3, i4;
    i1 = m_quad_indices.at(q);
    i2 = m_quad_indices.at(q+1);
    i3 = m_quad_indices.at(q+2);
    i4 = m_quad_indices.at(q+3);

    // recuperation des points
    Vec3 A, B, C, D;
    A = m_points.at(i1);
    B = m_points.at(i2);
    C = m_points.at(i3);
    D = m_points.at(i4);


    // calcul de la normale
    Vec3 n = normal_of(A, B, C);

    // calcul de la hauteur
    float longueur = sqrt(quad_aire(q));

    // calcul et ajout des 4 nouveaux points
    Vec3 p1, p2, p3, p4;
    p1 = A + n * longueur;
    p2 = B + n * longueur;
    p3 = C + n * longueur;
    p4 = D + n * longueur;

    int v1, v2, v3, v4;
    v1 = add_vertex(p1);
    v2 = add_vertex(p2);
    v3 = add_vertex(p3);
    v4 = add_vertex(p4);

    // on remplace le quad initial par le quad du dessu
    m_quad_indices.at(q) = v1;
    m_quad_indices.at(q+1) = v2;
    m_quad_indices.at(q+2) = v3;
    m_quad_indices.at(q+3) = v4;

    // on ajoute les 4 quads des cotes
    add_quad(i1, i2, v2, v1);
    add_quad(i3, i4, v4, v3);
    add_quad(i4, i1, v1, v4);
    add_quad(i2, i3, v3, v2);


    gl_update();
}

void MeshQuad::transfo_quad(int q, const glm::mat4& tr)
{
    // recuperation des indices de points
    int i1, i2, i3, i4;
    i1 = m_quad_indices.at(q);
    i2 = m_quad_indices.at(q+1);
    i3 = m_quad_indices.at(q+2);
    i4 = m_quad_indices.at(q+3);

    // recuperation des (references de) points
    Vec3& A = m_points.at(i1);
    Vec3& B = m_points.at(i2);
    Vec3& C = m_points.at(i3);
    Vec3& D = m_points.at(i4);


    // generation de la matrice de transfo globale:
    // indice utilisation de glm::inverse() et de local_frame
    Mat4 local = local_frame(q);

      // Si la l'inverse n'existe pas on retourne
     if (glm::determinant(local) == 0) {
        return;
    }
    Mat4 transfo = local * tr * glm::inverse(local);

    // Application au 4 points du quad
     
    A = Vec3(transfo * Vec4(A, 1));
    B = Vec3(transfo * Vec4(B, 1));
    C = Vec3(transfo * Vec4(C, 1));
    D = Vec3(transfo * Vec4(D, 1));

    gl_update();
}

void MeshQuad::decale_quad(int q, float d)
{
     transfo_quad(q, translate(0, 0, d));
     gl_update();
}

void MeshQuad::shrink_quad(int q, float s)
{
    transfo_quad(q, scale(s));
    gl_update();
}

void MeshQuad::tourne_quad(int q, float a)
{
     transfo_quad(q, rotateZ(a));
     gl_update();
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

