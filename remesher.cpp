#include "remesher.h"

//#include "RemesherPlugin.hh"
#include "Algorithms/AdaptiveRemesherT.hh"
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalDefines.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
Remesher::Remesher()
{
}


void Remesher::adaptiveRemeshing(){
    double error = 0;
    double min_edge = 0;
    double max_edge = 0;
    unsigned int iters =0;
    bool projection = false;
    bool vertexSelection = false;

    slotAdaptiveRemeshing(0, error, min_edge, max_edge, iters, projection,vertexSelection);

}

void Remesher::slotAdaptiveRemeshing(int _objectId, double _error, double _min_edge_length, double _max_edge_length, unsigned int _iters, bool _use_projection, bool _vertex_selection){
    operation_ = REMESH_ADAPTIVE;
    TriMesh* mesh = PluginFunctions::triMesh(object);
    Remeshing::AdaptiveRemesherT<TriMesh> remesher(*mesh, progress_);

    Remeshing::BaseRemesherT<TriMesh>::Selection selection = (_vertex_selection) ? Remeshing::BaseRemesherT<TriMesh>::VERTEX_SELECTION : Remeshing::BaseRemesherT<TriMesh>::FACE_SELECTION;

    remesher.remesh(_error, _min_edge_length, _max_edge_length, _iters, _use_projection, selection);

    mesh->update_normals();

}


void Remesher::computeInitValues(){
    double mean_edge = 0.0;
      double max_feature_angle = 0.0;
      for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, DataType(DATA_TRIANGLE_MESH | DATA_POLY_MESH)) ;
            o_it != PluginFunctions::objectsEnd(); ++o_it)  {

          if(o_it->dataType() == DATA_TRIANGLE_MESH) {

            TriMesh* mesh = PluginFunctions::triMesh(o_it->id());
            if(!mesh) return;

            mesh->update_face_normals();

            for(TriMesh::EdgeIter e_it = mesh->edges_begin(); e_it != mesh->edges_end(); ++e_it) {

              TriMesh::HalfedgeHandle he = mesh->halfedge_handle(*e_it, 0);

              ACG::Vec3d vec_e = mesh->point(mesh->to_vertex_handle(he)) - mesh->point(mesh->from_vertex_handle(he));

              mean_edge += vec_e.length();

              // Estimate feature angle
              TriMesh::FaceHandle fh1 = mesh->face_handle(he);
              TriMesh::FaceHandle fh2 = mesh->face_handle(mesh->opposite_halfedge_handle(he));

              // Boundary halfedge?
              if ( !fh1.is_valid() || !fh2.is_valid() )
                continue;

              TriMesh::Normal n1 = mesh->normal(fh1);
              TriMesh::Normal n2 = mesh->normal(fh2);

              double feature_angle = (1.0 - (n1 | n2));

              if(feature_angle > max_feature_angle) max_feature_angle = feature_angle;
            }

            mean_edge /= (double)mesh->n_edges();


}
