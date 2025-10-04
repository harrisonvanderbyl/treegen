#include "treegen2.h"






void TreeGen2::updateParts() {
	
}



Dictionary TreeGen2::createTree() const{
	Transform3D startmatrix = Transform3D(Basis());
	Transform3D secondMatrix = Transform3D(Basis(),Vector3(0,1,0));
	int depth = splits.z;

	Array mtransforms;
	// scale, rotate, move
	Transform3D rootTransform = Transform3D(Basis(), Vector3(0, 0, 0));
	Transform3D baseTransform = Transform3D(Basis(), Vector3(0, 1, 0));

	Transform3D mymat = Transform3D();
	mymat.scale(Vector3(0.9,0.9,0.9));
	mymat.rotate(Vector3(1.0,0.0,0.0), 0.1);
	mymat.rotate(Vector3(0.0,1.0,0.0), 0.1);

	for(int i = 0; i < depth; i++){

	}

	auto dict = Dictionary();

	dict["mats"] = mtransforms;
	return dict;
}

void TreeGen2::_create_mesh_array(Array &p_arr, Dictionary tr) const {

	
	
	
	// if(splits.is_valid()){
	Array vertexes;
	Array indexes;


		

		
	// 	p_arr[ARRAY_VERTEX] = meshdata[ARRAY_VERTEX];
	// 	p_arr[ARRAY_NORMAL] = meshdata[ARRAY_NORMAL];
	// 	p_arr[ARRAY_TANGENT] = meshdata[ARRAY_TANGENT];
	// 	p_arr[ARRAY_TEX_UV] = meshdata[ARRAY_TEX_UV];
	// 	p_arr[ARRAY_INDEX] = meshdata[ARRAY_INDEX];
	// }
}
