/*
 *  Progressive Mesh type Polygon Reduction Algorithm
 *  by Stan Melax (c) 1998
 *  Permission to use any of this code wherever you want is granted..
 *  Although, please do acknowledge authorship if appropriate.
 * https://dev.gameres.com/Program/Visual/3D/PolygonReduction.pdf
 * https://github.com/andandandand/progressive-mesh-reduction-with-edge-collapse
 *  See the header file progmesh.h for a description of this module
 */

#include <Types.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>

class tridata
{
public:
    int v[3];  // indices to vertex list
    // texture and vertex normal info removed for this demo
};

/*
 *  For the polygon reduction algorithm, we use data structures
 *  that contain more information than the
 *  indexed face set type of data structure.
 *  From a vertex, we wish to be able to quickly get the
 *  neighbor faces and vertices.
 */
class Triangle;
class Vertex;

class Triangle
{
public:
    Vertex* vertex[3];  // the 3 points that make a triangle
    vec3 normal;        // unit vector othogonal to a face
    Triangle(Vertex* v0, Vertex* v1, Vertex* v2);
    ~Triangle();
    void ComputeNormal();
    void ReplaceVertex(Vertex* vold, Vertex* vnew);
    int HasVertex(Vertex* v);
};
class Vertex
{
public:
    vec3 position;                  // location of point in euclidean space
    int id;                         // place of vertex in original list
    std::vector<Vertex*> neighbor;  // adjacent vertices
    std::vector<Triangle*> face;    // adjacent triangles
    float objdist;                  // cached cost of collapsing edge
    Vertex* collapse;               // candidate vertex for collapse
    Vertex(const vec3& v, int _id);
    ~Vertex();
    void RemoveIfNonNeighbor(Vertex* n);
};
template <typename T>
void remove(std::vector<T>& v, const T& t)
{
    auto iter = std::find(v.begin(), v.end(), t);
    if (iter != v.end())
    {
        v.erase(iter);
    }
}
template <typename T>
int contains(std::vector<T>& v, const T& t)
{
    int count = 0;
    for (const auto& value : v)
    {
        if (value == t)
        {
            ++count;
        }
    }
    return count;
}

template <typename T>
void addUnique(std::vector<T>& v, const T& t)
{
    auto iter = std::find(v.begin(), v.end(), t);
    if (iter == v.end())
    {
        v.push_back(t);
    }
}

std::vector<Vertex*> vertices;
std::vector<Triangle*> triangles;

Triangle::Triangle(Vertex* v0, Vertex* v1, Vertex* v2)
{
    assert(v0 != v1 && v1 != v2 && v2 != v0);  //#mod1
    vertex[0] = v0;
    vertex[1] = v1;
    vertex[2] = v2;
    ComputeNormal();
    triangles.push_back(this);
    for (int i = 0; i < 3; i++)
    {
        vertex[i]->face.push_back(this);
        for (int j = 0; j < 3; j++)
            if (i != j)
            {
                addUnique(vertex[i]->neighbor, vertex[j]);
            }
    }
}
Triangle::~Triangle()
{
    int i = 0;
    remove(triangles, this);

    for (i = 0; i < 3; i++)
    {
        if (vertex[i])
            remove(vertex[i]->face, this);
    }
    for (i = 0; i < 3; i++)
    {
        int i2 = (i + 1) % 3;
        if (!vertex[i] || !vertex[i2])
            continue;
        vertex[i]->RemoveIfNonNeighbor(vertex[i2]);
        vertex[i2]->RemoveIfNonNeighbor(vertex[i]);
    }
}
int Triangle::HasVertex(Vertex* v)
{
    return (v == vertex[0] || v == vertex[1] || v == vertex[2]);
}
void Triangle::ComputeNormal()
{
    vec3 v0 = vertex[0]->position;
    vec3 v1 = vertex[1]->position;
    vec3 v2 = vertex[2]->position;
    normal  = (v1 - v0) * (v2 - v1);
    if (glm::length(normal) == 0)
        return;
    normal = normalize(normal);
}
void Triangle::ReplaceVertex(Vertex* vold, Vertex* vnew)
{
    assert(vold && vnew);
    assert(vold == vertex[0] || vold == vertex[1] || vold == vertex[2]);
    assert(vnew != vertex[0] && vnew != vertex[1] && vnew != vertex[2]);
    if (vold == vertex[0])
    {
        vertex[0] = vnew;
    }
    else if (vold == vertex[1])
    {
        vertex[1] = vnew;
    }
    else
    {
        assert(vold == vertex[2]);
        vertex[2] = vnew;
    }
    int i = 0;
    remove(vold->face, this);
    assert(!contains(vnew->face, this));
    vnew->face.push_back(this);
    for (i = 0; i < 3; i++)
    {
        vold->RemoveIfNonNeighbor(vertex[i]);
        vertex[i]->RemoveIfNonNeighbor(vold);
    }
    for (i = 0; i < 3; i++)
    {
        assert(contains(vertex[i]->face, this) == 1);
        for (int j = 0; j < 3; j++)
        {
            if (i != j)
            {
                addUnique(vertex[i]->neighbor, vertex[j]);
            }
        }
    }
    ComputeNormal();
}

Vertex::Vertex(const vec3& v, int _id)
{
    position = v;
    id       = _id;
    vertices.push_back(this);
}

Vertex::~Vertex()
{
    // assert(face.num == 0);
    while (neighbor.size())
    {
        remove(neighbor.front()->neighbor, this);
        remove(neighbor, neighbor.front());
    }
    remove(vertices, this);
}
void Vertex::RemoveIfNonNeighbor(Vertex* n)
{
    // removes n from neighbor list if n isn't a neighbor.
    if (!contains(neighbor, n))
        return;
    for (size_t i = 0; i < face.size(); i++)
    {
        if (face[i]->HasVertex(n))
            return;
    }
    remove(neighbor, n);
}

float ComputeEdgeCollapseCost(Vertex* u, Vertex* v)
{
    // if we collapse edge uv by moving u to v then how
    // much different will the model change, i.e. how much "error".
    // Texture, vertex normal, and border vertex code was removed
    // to keep this demo as simple as possible.
    // The method of determining cost was designed in order
    // to exploit small and coplanar regions for
    // effective polygon reduction.
    // Is is possible to add some checks here to see if "folds"
    // would be generated.  i.e. normal of a remaining face gets
    // flipped.  I never seemed to run into this problem and
    // therefore never added code to detect this case.
    size_t i{0};
    float edgelength = glm::length(v->position - u->position);
    float curvature  = 0;

    // find the "sides" triangles that are on the edge uv
    std::vector<Triangle*> sides;
    for (i = 0; i < u->face.size(); i++)
    {
        if (u->face[i]->HasVertex(v))
        {
            sides.push_back(u->face[i]);
        }
    }
    // use the triangle facing most away from the sides
    // to determine our curvature term
    for (i = 0; i < u->face.size(); i++)
    {
        float mincurv = 1;  // curve for face i and closer side to it
        for (size_t j = 0; j < sides.size(); j++)
        {
            // use dot product of face normals. '^' defined in vector
            float dotprod = glm::dot(u->face[i]->normal, sides[j]->normal);
            mincurv       = glm::min(mincurv, (1 - dotprod) / 2.0f);
        }
        curvature = glm::max(curvature, mincurv);
    }
    // the more coplanar the lower the curvature term
    return edgelength * curvature;
}

void ComputeEdgeCostAtVertex(Vertex* v)
{
    // compute the edge collapse cost for all edges that start
    // from vertex v.  Since we are only interested in reducing
    // the object by selecting the min cost edge at each step, we
    // only cache the cost of the least cost edge at this vertex
    // (in member variable collapse) as well as the value of the
    // cost (in member variable objdist).
    if (v->neighbor.size() == 0)
    {
        // v doesn't have neighbors so it costs nothing to collapse
        v->collapse = nullptr;
        v->objdist  = -0.01f;
        return;
    }
    v->objdist  = 1000000;
    v->collapse = nullptr;
    // search all neighboring edges for "least cost" edge
    for (size_t i = 0; i < v->neighbor.size(); i++)
    {
        float dist = ComputeEdgeCollapseCost(v, v->neighbor[i]);
        if (dist < v->objdist)
        {
            v->collapse = v->neighbor[i];  // candidate for edge collapse
            v->objdist  = dist;            // cost of the collapse
        }
    }
}
void ComputeAllEdgeCollapseCosts()
{
    // For all the edges, compute the difference it would make
    // to the model if it was collapsed.  The least of these
    // per vertex is cached in each vertex object.
    for (size_t i = 0; i < vertices.size(); i++)
    {
        ComputeEdgeCostAtVertex(vertices[i]);
    }
}

void Collapse(Vertex* u, Vertex* v)
{
    // Collapse the edge uv by moving vertex u onto v
    // Actually remove tris on uv, then update tris that
    // have u to have v, and then remove u.
    if (!v)
    {
        // u is a vertex all by itself so just delete it
        delete u;
        return;
    }
    size_t i{0};
    std::vector<Vertex*> tmp;
    // make tmp a list of all the neighbors of u
    for (i = 0; i < u->neighbor.size(); i++)
    {
        tmp.push_back(u->neighbor[i]);
    }
    // delete triangles on edge uv:
    for (i = u->face.size() - 1; i == 0; i--)
    {
        if (u->face[i]->HasVertex(v))
        {
            delete (u->face[i]);
        }
    }
    // update remaining triangles to have v instead of u
    for (i = u->face.size() - 1; i == 0; i--)
    {
        u->face[i]->ReplaceVertex(u, v);
    }
    delete u;
    // recompute the edge collapse costs for neighboring vertices
    for (i = 0; i < tmp.size(); i++)
    {
        ComputeEdgeCostAtVertex(tmp[i]);
    }
}

void AddVertex(const std::vector<vec3>& vert)
{
    for (size_t i = 0; i < vert.size(); i++)
    {
       // Vertex* v = new Vertex(vert[i], i);
    }
}
void AddFaces(std::vector<tridata>& tri)
{
    for (size_t i = 0; i < tri.size(); i++)
    {
       // Triangle* t = new Triangle(vertices[tri[i].v[0]], vertices[tri[i].v[1]], vertices[tri[i].v[2]]);
    }
}

Vertex* MinimumCostEdge()
{
    // Find the edge that when collapsed will affect model the least.
    // This funtion actually returns a Vertex, the second vertex
    // of the edge (collapse candidate) is stored in the vertex data.
    // Serious optimization opportunity here: this function currently
    // does a sequential search through an unsorted list :-(
    // Our algorithm could be O(n*lg(n)) instead of O(n*n)
    Vertex* mn = vertices[0];
    for (size_t i = 0; i < vertices.size(); i++)
    {
        if (vertices[i]->objdist < mn->objdist)
        {
            mn = vertices[i];
        }
    }
    return mn;
}

void ProgressiveMesh(std::vector<vec3>& vert, std::vector<tridata>& tri, std::vector<int>& map,
                     std::vector<int>& permutation)
{
    AddVertex(vert);  // put input data into our data structures
    AddFaces(tri);
    ComputeAllEdgeCollapseCosts();        // cache all edge collapse costs
    permutation.resize(vertices.size());  // allocate space
    map.resize(vertices.size());          // allocate space
    // reduce the object down to nothing:
    while (vertices.size() > 0)
    {
        // get the next vertex to collapse
        Vertex* mn = MinimumCostEdge();
        // keep track of this vertex, i.e. the collapse ordering
        permutation[mn->id] = vertices.size() - 1;
        // keep track of vertex to which we collapse to
        map[vertices.size() - 1] = (mn->collapse) ? mn->collapse->id : -1;
        // Collapse this edge
        Collapse(mn, mn->collapse);
    }
    // reorder the map list based on the collapse ordering
    for (size_t i = 0; i < map.size(); i++)
    {
        map[i] = (map[i] == -1) ? 0 : permutation[map[i]];
    }
    // The caller of this function should reorder their vertices
    // according to the returned "permutation".
}
