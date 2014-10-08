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
// Mesh type
typedef OpenMesh::TriMesh_ArrayKernelT<> MyMesh;

void writeMesh(MyMesh mesh,std::string outputFilename){
    if (!OpenMesh::IO::write_mesh(mesh, outputFilename))
    {
        std::cerr << "write error\n";
        exit(1);
    }
    cout << "Output file created : "<< outputFilename<<endl;
}
MyMesh cleanMesh(MyMesh mesh){
    int i=0;
    for (MyMesh::FaceIter f_it=mesh.faces_begin(); f_it!=mesh.faces_end(); ++f_it){
        if (mesh.valence(f_it.handle())==0){
            mesh.delete_face(f_it.handle(),false);
            i++;
        }
    }
    mesh.garbage_collection();

    std::cout << "Faces deleted " << i++ << std::endl;
    return mesh;
}


float computeMeshVolume(MyMesh mesh){
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
    return sum;
}


MyMesh loadMesh(std::string filename){
    // ---------------------------------------- necessary types

    // ---------------------------------------- decimater setup
    MyMesh mesh; // a mesh object

    //std::string filename = "fandisk.obj";

    OpenMesh::IO::Options opt;
    if (!OpenMesh::IO::read_mesh(mesh, filename,opt))
    {
        std::cerr << "read error\n";
        exit(1);
    }

    return mesh;
}

void createLOD(std::string filename,float percent){

    MyMesh mesh=loadMesh(filename);
    float sum1 = computeMeshVolume(mesh);
    // Decimater type
    typedef OpenMesh::Decimater::DecimaterT< MyMesh > Decimater;
    // Decimation Module Handle type
    typedef OpenMesh::Decimater::ModQuadricT< MyMesh >::Handle HModQuadric;

    Decimater decimater(mesh); // a decimater object, connected to a mesh

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
    float vertD = roundf(percent*decimater.mesh().n_vertices());
    decimater.decimate_to(vertD);

    printf ("decimate to %f percent (%f v) of %zu vertices\n", percent, vertD,decimater.mesh().n_vertices());

    mesh=cleanMesh(mesh);
    float sum2 = computeMeshVolume(mesh);
    float diff=sum1 - sum2;
    printf("difference sum (%f) - sum 2 (%f) : %f\n",sum1,sum2,diff);

    string output= to_string(percent)+filename+"";
    writeMesh(mesh,output);

}



int main(int argc, char *argv[]){

    vector<float> *LOD = new vector<float>();
    std::string filename = argv[1];

    for (int i =2;i<argc;i++){
        //
        float lodpercent=atof(argv[i]);
        LOD->push_back(lodpercent);
    }

    for (int i =0;i<LOD->size();i++){

        createLOD(filename,LOD->at(i));
    }


    return 0;
}
