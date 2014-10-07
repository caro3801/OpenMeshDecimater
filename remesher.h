#ifndef REMESHER_H
#define REMESHER_H

class Remesher
{
public:
    Remesher();
private :
    enum RemeshingOperation { REMESH_UNIFORM, REMESH_ADAPTIVE };
    RemeshingOperation operation_;


    void adaptiveRemeshingButtonClicked();

    void adaptiveRemeshing();


    void computeInitValues();

    void slotAdaptiveRemeshing(int             _objectId,
                                   double        _error,
                                   double        _min_edge_length,
                                   double        _max_edge_length,
                                   unsigned int  _iters,
                                   bool          _use_projection = true,
                                   bool          _vertex_selection = true);

};

#endif // REMESHER_H
