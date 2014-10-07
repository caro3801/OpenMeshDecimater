#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Tools/Decimater/DecimaterT.hh>
#include <OpenMesh/Tools/Decimater/ModQuadricT.hh>
#include <OpenMesh/Core/Mesh/Handles.hh>

#include <vector>
//
using namespace std;


template<class T>
inline vector<T> cross(const vector<T>& v1,const vector<T>& v2) // 6 mults, 3 adds
{return vector<T>({v1[1]*v2[2]-v1[2]*v2[1],v1[2]*v2[0]-v1[0]*v2[2],v1[0]*v2[1]-v1[1]*v2[0]});}
template<class T>
inline T dot(const vector<T>& v1,const vector<T>& v2)
{return v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2];}
template<class T>
inline T determinant(const vector<T>& u,const vector<T>& v,const vector<T>& w)
{return dot(u,cross(v,w));}

int main(int argc, char *argv[]){

    int decimate_to=1000;
    decimate_to =atoi(argv[1]);

    printf ("decimate to %d vertices\n", decimate_to);
    // ---------------------------------------- necessary types
    // Mesh type
    typedef OpenMesh::TriMesh_ArrayKernelT<> MyMesh;
    // Decimater type
    typedef OpenMesh::Decimater::DecimaterT< MyMesh > Decimater;
    // Decimation Module Handle type
    typedef OpenMesh::Decimater::ModQuadricT< MyMesh >::Handle HModQuadric;
    // ---------------------------------------- decimater setup
    MyMesh mesh; // a mesh object
    std::string filename = "fandisk.obj";

    OpenMesh::IO::Options opt;
    if (!OpenMesh::IO::read_mesh(mesh, filename,opt))
    {
        std::cerr << "read error\n";
        exit(1);
    }
    float sum=0;
    for (MyMesh::FaceIter f_it=mesh.faces_begin(); f_it!=mesh.faces_end(); ++f_it){

        std::vector< std::vector<float> > face ;
        int i=0;
        for (MyMesh::FaceVertexIter fv_it = mesh.fv_begin(f_it.handle()); fv_it; ++fv_it)
        {
            MyMesh::Point point=mesh.point(fv_it);
            face.push_back({point[0],point[1],point[2]});
            i++;
        }
        sum+= determinant(face[0],face[1],face[2]);
    }

    cout << "sum " <<sum <<std::endl;
    Decimater decimater(mesh); // a decimater object, connected to a mesh

    cout << decimater.mesh().n_vertices()<<endl;
    HModQuadric hModQuadric; // use a quadric module
    decimater.add(hModQuadric); // register module at the decimater
    // the way to access the module
    /*
    * since we need exactly one priority module (non-binary)
    * we have to call set_binary(false) for our priority module
    * in the case of HModQuadric, unset_max_err() calls set_binary(false) internally
    */
    decimater.module( hModQuadric ).unset_max_err();
    decimater.initialize(); // let the decimater initialize the mesh and the
    // modules

    decimater.decimate_to(500);
    int i=0;
    float sum2=0;
    for (MyMesh::FaceIter f_it=mesh.faces_begin(); f_it!=mesh.faces_end(); ++f_it){



        if (mesh.valence(f_it.handle())==0){
            mesh.delete_face(f_it.handle(),false);
            i++;
        }else{
            std::vector< std::vector<float> > face ;
            for (MyMesh::FaceVertexIter fv_it = mesh.fv_begin(f_it.handle()); fv_it; ++fv_it)
            {
                MyMesh::Point point=mesh.point(fv_it);
                face.push_back({point[0],point[1],point[2]});
                i++;
            }
            sum2+= determinant(face[0],face[1],face[2]);
        }

    }


    cout << "sum2 " <<sum2 <<std::endl;
    float diff=sum - sum2;
cout << "difference sum - sum 2 " << diff <<std::endl;
    mesh.garbage_collection();

    cout << decimater.mesh().n_vertices()<<endl;
    std::cout << "Faces deleted " << i++ << std::endl;
    if (!OpenMesh::IO::write_mesh(mesh, "fandiskTest.obj"))
    {
        std::cerr << "write error\n";
        exit(1);
    }
    return 0;
}
