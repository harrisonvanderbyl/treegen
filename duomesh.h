#ifndef DUOMESH_H
#define DUOMESH_H

#include "flowergen.h"
#include "scene/resources/mesh.h"
#include "scene/resources/primitive_meshes.h"

#include "servers/rendering_server.h"
class DuoMesh : public Mesh {
	GDCLASS(DuoMesh, Mesh);

private:
	RID mesh;
	mutable AABB aabb;
	AABB custom_aabb;
	Ref<FlowerGen> mesh_a = 0;
	Ref<Material> material;
	bool flip_faces;

	mutable int array_len = 0;
	mutable int index_array_len = 0;
	mutable int array_len_a = 0;
	mutable int index_array_len_a = 0;
	mutable bool pending_request;
	void _update() const;

protected:
	Mesh::PrimitiveType primitive_type;

	static void _bind_methods();

	virtual void _create_mesh_array(Array &p_arr, Dictionary tr) const = 0;
	void _request_update();

public:
	virtual int get_surface_count() const;
	virtual int surface_get_array_len(int p_idx) const;
	virtual int surface_get_array_index_len(int p_idx) const;
	virtual Array surface_get_arrays(int p_surface) const;
	virtual TypedArray<Array> surface_get_blend_shape_arrays(int p_surface) const;
	virtual BitField<Mesh::ArrayFormat> surface_get_format(int p_idx) const;
	virtual Mesh::PrimitiveType surface_get_primitive_type(int p_idx) const;
	virtual void surface_set_material(int p_idx, const Ref<Material> &p_material);
	virtual Ref<Material> surface_get_material(int p_idx) const;
	virtual int get_blend_shape_count() const;
	virtual StringName get_blend_shape_name(int p_index) const;
	virtual void set_blend_shape_name(int p_index, const StringName &p_name);
	virtual AABB get_aabb() const;
	virtual RID get_rid() const;
	Dictionary surface_get_lods(int p_surface) const {
		return Dictionary(); //not really supported
	}
	virtual Dictionary createTree() const = 0;
	Array leaftransforms;
	void set_mesh_a(const Ref<FlowerGen> &p_mesh);
	Ref<FlowerGen> get_mesh_a() const;

	void set_material(const Ref<Material> &p_material);
	Ref<Material> get_material() const;

	Array get_mesh_arrays() const;

	void set_custom_aabb(const AABB &p_custom);
	AABB get_custom_aabb() const;

	void set_flip_faces(bool p_enable);
	bool get_flip_faces() const;

	DuoMesh() {
		flip_faces = false;
		// defaults

		mesh = RenderingServer::get_singleton()->mesh_create();

		// assume primitive triangles as the type, correct for all but one and it will change this :)
		primitive_type = Mesh::PRIMITIVE_TRIANGLES;

		// make sure we do an update after we've finished constructing our object
		pending_request = true;
		leaftransforms = Array();
		leaftransforms.push_back(Transform3D(Basis(0, 0, 1, 0, 1, 0, 1, 0, 0), Vector3(0, 0, 0)));
	}
	~DuoMesh() {
		RenderingServer::get_singleton()->free(mesh);
	}
};

#endif
