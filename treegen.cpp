#include "treegen.h"

Transform3D IDENTITY = Transform3D(Basis(1, 0, 0, 0, 1, 0, 0, 0, 1), Vector3(0, 0, 0));
void TreeGen::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_sections", "sections"), &TreeGen::set_sections);
	ClassDB::bind_method(D_METHOD("get_sections"), &TreeGen::get_sections);
	ClassDB::bind_method(D_METHOD("set_distribution", "distribution"), &TreeGen::set_distribution);
	ClassDB::bind_method(D_METHOD("get_distribution"), &TreeGen::get_distribution);
	ClassDB::bind_method(D_METHOD("set_auto_split", "auto_split"), &TreeGen::set_auto_split);
	ClassDB::bind_method(D_METHOD("get_auto_split"), &TreeGen::get_auto_split);
	ClassDB::bind_method(D_METHOD("set_splits", "splitsB"), &TreeGen::set_splits);
	ClassDB::bind_method(D_METHOD("get_splits"), &TreeGen::get_splits);
	ClassDB::bind_method(D_METHOD("set_rotation", "rotation"), &TreeGen::set_rotation);
	ClassDB::bind_method(D_METHOD("get_rotation"), &TreeGen::get_rotation);
	ClassDB::bind_method(D_METHOD("set_radius_scale", "radius_scale"), &TreeGen::set_radius_scale);
	ClassDB::bind_method(D_METHOD("get_radius_scale"), &TreeGen::get_radius_scale);
	ClassDB::bind_method(D_METHOD("set_radius_mult", "radius_mult"), &TreeGen::set_radius_mult);
	ClassDB::bind_method(D_METHOD("get_radius_mult"), &TreeGen::get_radius_mult);
	ClassDB::bind_method(D_METHOD("gen", "w", "h", "home"), &TreeGen::gen);
	ClassDB::bind_method(D_METHOD("addtreebranch", "inf"), &TreeGen::addtreebranch);
	ClassDB::bind_method(D_METHOD("createTree"), &TreeGen::createTree);
	ClassDB::bind_method(D_METHOD("set_radius", "radius"), &TreeGen::set_radius);
	ClassDB::bind_method(D_METHOD("get_radius"), &TreeGen::get_radius);
	ClassDB::bind_method(D_METHOD("set_radial_segments", "segments"), &TreeGen::set_radial_segments);
	ClassDB::bind_method(D_METHOD("get_radial_segments"), &TreeGen::get_radial_segments);
	ClassDB::bind_method(D_METHOD("set_rings", "rings"), &TreeGen::set_rings);
	ClassDB::bind_method(D_METHOD("get_rings"), &TreeGen::get_rings);
	ClassDB::bind_method(D_METHOD("set_leafScale", "leafScale"), &TreeGen::set_leafScale);
	ClassDB::bind_method(D_METHOD("get_leafScale"), &TreeGen::get_leafScale);
	ClassDB::bind_method(D_METHOD("get_transforms"), &TreeGen::get_transforms);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sections", PROPERTY_HINT_RANGE, "2.0,50.0,0.01"), "set_sections", "get_sections");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "distribution", PROPERTY_HINT_RANGE, "0.0,2.0,0.01"), "set_distribution", "get_distribution");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "auto_split", PROPERTY_HINT_RANGE, "0.0,2.0,0.01"), "set_auto_split", "get_auto_split");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_VECTOR3_ARRAY, "splits", PROPERTY_HINT_NONE, ""), "set_splits", "get_splits");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "rotation", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_rotation", "get_rotation");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radius_scale", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_radius_scale", "get_radius_scale");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radius_mult", PROPERTY_HINT_RANGE, "-2.0,2.0,0.01"), "set_radius_mult", "get_radius_mult");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radius", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_radius", "get_radius");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radial_segments", PROPERTY_HINT_RANGE, "3.0,20.0,1.0"), "set_radial_segments", "get_radial_segments");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "rings", PROPERTY_HINT_RANGE, "0.0,10.0,0.01"), "set_rings", "get_rings");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "leafScale", PROPERTY_HINT_RANGE, "0.0,20.0,0.01"), "set_leafScale", "get_leafScale");
}
void TreeGen::set_sections(int sections) {
	this->sections = sections;
	_request_update();
}
int TreeGen::get_sections() const {
	return sections;
}
void TreeGen::set_distribution(float distribution) {
	this->distribution = distribution;
	_request_update();
}
float TreeGen::get_distribution() const {
	return distribution;
}
void TreeGen::set_auto_split(float auto_split) {
	this->auto_split = auto_split;
	Vector<Vector3> temp = Vector<Vector3>();
	if (this->splits.size() < 2) {
		this->splits = Vector<Vector3>();
		this->splits.push_back(Vector3(0, 0, 0));
		this->splits.push_back(Vector3(0, 0, 0));
	}
	for (int i = 0; i < splits.size(); i++) {
		float tp = float(i) / float(splits.size());
		float atp = ((tp - 0.5) * float(distribution)) + 0.5;
		temp.append(Vector3(atp * auto_split, 0, 0));
	}
	this->splits = temp;
	this->updateParts();
	_request_update();
}
float TreeGen::get_auto_split() const {
	return auto_split;
}
void TreeGen::set_splits(PackedVector3Array splits) {
	PackedVector3Array temp = PackedVector3Array();

	float tot = 0.0;
	for (int i = 0; i < splits.size(); i++) {
		tot += splits[i].y * splits[i].y;
	}
	tot = MAX(sqrt(tot), 0.0001);
	for (int i = 0; i < splits.size(); i++) {
		temp.append(Vector3(splits[i].x, splits[i].y / tot, splits[i].z));
	}
	this->splits = temp;
	this->updateParts();
	_request_update();
}
PackedVector3Array TreeGen::get_splits() const {
	return splits;
}
void TreeGen::set_rotation(float rotation) {
	this->rotation = rotation;
	this->updateParts();
	_request_update();
}
float TreeGen::get_rotation() const {
	return rotation;
}
void TreeGen::set_radius_scale(float radius_scale) {
	this->radius_scale = radius_scale;
	_request_update();
}
float TreeGen::get_radius_scale() const {
	return radius_scale;
}
void TreeGen::set_radius_mult(float radius_mult) {
	this->radius_mult = radius_mult;
	_request_update();
}
float TreeGen::get_radius_mult() const {
	return radius_mult;
}
void TreeGen::set_radius(float radius) {
	this->radius = radius;
	_request_update();
}
float TreeGen::get_radius() const {
	return radius;
}

void TreeGen::set_height(float height) {
	this->height = height;
	_request_update();
}
float TreeGen::get_height() const {
	return height;
}
void TreeGen::set_width(float width) {
	this->width = width;
	_request_update();
}
float TreeGen::get_width() const {
	return width;
}
void TreeGen::set_radial_segments(int segments) {
	this->radial_segments = segments;
	this->updateParts();
	_request_update();
}
int TreeGen::get_radial_segments() const {
	return radial_segments;
}
void TreeGen::set_rings(int rings) {
	this->rings = rings;
	_request_update();
}
int TreeGen::get_rings() const {
	return rings;
}
void TreeGen::set_leafScale(float leafScale) {
	this->leafScale = leafScale;
	_request_update();
}
float TreeGen::get_leafScale() const {
	return leafScale;
}
Array TreeGen::get_transforms() {
	if (!transforms.is_empty())
		return transforms;
	else {
		Dictionary tr = createTree();
		Array tree = tr["tree"];
		Array leaf = tr["leaf"];
		transforms = tree;
		return tree;
	}
}

Dictionary TreeGen::gen(float w, float h, Transform3D home) const {
	Dictionary thisbranch = Dictionary{};
	Transform3D az = home.translated_local((Vector3(0.0, h, 0.0)));
	thisbranch["branches"] = Array{};
	for (int i = 0; i < splits.size(); i++) {
		Dictionary z = Dictionary{};
		float tp = float(i) / float(splits.size());
		float atp = ((tp - 0.5) * float(distribution)) + 0.5;
		z["tran"] = (az * Transform3D(sid[i]));
		z["w"] = sqrt((w * w) * float(radius_scale) * (1.0 - atp));
		z["h"] = sqrt(h * h * (1.0 - atp) * float(radius_mult));
		Array(thisbranch["branches"]).append(z);
	}
	Transform3D t = home.translated_local(Vector3(0.0, h / 2, 0.0)) * IDENTITY.scaled(Vector3(w, h, w));
	thisbranch["myTransform"] = t;

	thisbranch["leaftransform"] = home.translated_local(Vector3(0.0, h / 2, 0.0)) * IDENTITY.scaled(Vector3(leafScale, leafScale, leafScale));
	return thisbranch;
}
Dictionary TreeGen::addtreebranch(Dictionary inf) const {
	Array leaftransforms = Array{};
	Array treetransforms = Array{};
	Array parentTransforms = Array{};
	Transform3D tree = Dictionary(inf["b"])["myTransform"];
	Transform3D leaf = Dictionary(inf["b"])["leaftransform"];
	if (int(inf["d"]) > 0) {
		for (int bi = 0; bi < Array(Dictionary(inf["b"])["branches"]).size(); bi++) {
			Dictionary b = Array(Dictionary(inf["b"])["branches"])[bi];
			Dictionary a = gen(b["w"], b["h"], b["tran"]);
			//Transform zz = a["myTransform"];
			if (float(b["w"]) > 0.5) {
				Dictionary n = Dictionary{};
				a["parent"] = tree * Transform3D(sidL[bi]);
				n["b"] = a;
				n["d"] = int(inf["d"]) - 1;

				Dictionary r = addtreebranch(n);
				Array nt = r["tree"];
				Array nl = r["leaf"];
				Array npt = r["parent"];
				for (int yy = 0; yy < nt.size(); yy++) {
					treetransforms.append(nt[yy]);
					parentTransforms.append(npt[yy]);
				}
				for (int xx = 0; xx < nl.size(); xx++) {
					leaftransforms.append(nl[xx]);
				}
			} else {
				if (bi == 0) {
					//leaf =  IDENTITY.translated_local(Vector3(0.0,-1.0,0.0))*leaf;
					leaftransforms.append(leaf);
				}
			}
		}
	} else {
		//leaf =  IDENTITY.translated_local(Vector3(0.0,-1.0,0.0))*leaf;
		leaftransforms.append(leaf);
	}

	treetransforms.append(tree);
	;
	Transform3D parent = Dictionary(inf["b"])["parent"];
	parentTransforms.append(parent);
	Dictionary ret = Dictionary{};
	ret["tree"] = treetransforms;
	ret["leaf"] = leaftransforms;
	ret["parent"] = parentTransforms;
	return ret;
}
Dictionary TreeGen::createTree() const {
	Dictionary b = gen(width, height, Transform3D(Basis(1, 0, 0, 0, 1, 0, 0, 0, 1), Vector3(0, 0, 0)));
	Dictionary z = Dictionary{};
	b["parent"] = Transform3D(Basis(1, 0, 0, 0, 1, 0, 0, 0, 1), Vector3(0, -0.5, 0)).scaled(Vector3(width, height, width));
	z["b"] = b;
	z["d"] = sections;

	Dictionary t = addtreebranch(z);
	return t;
}
void TreeGen::updateParts() {
	this->sid = Array();
	this->sidL = Array();
	for (int i = 0; i < splits.size(); i++) {
		float tp = float(i) / float(splits.size());
		sid.append(
				IDENTITY.rotated(Vector3(0.0, 0.0, 1.0), (float(splits[i].x) * 3.1415 / 2)).rotated(Vector3(0.0, 1.0, 0.0), 2.0 * ((tp + rotation) * 3.1415)));
		sidL.append(
				IDENTITY.rotated(Vector3(0, 1, 0), (2.0 * (round((float(i) / float(splits.size()) + rotation) * float(radial_segments)) / float(radial_segments)) * 3.1415)));
	}
}
void TreeGen::_create_mesh_array(Array &p_arr, Dictionary tr) const {
#define ADD_TANGENT(m_x, m_y, m_z, m_d) \
	tangents.push_back(m_x);            \
	tangents.push_back(m_y);            \
	tangents.push_back(m_z);            \
	tangents.push_back(m_d);

	p_arr[ARRAY_VERTEX] = Array();
	p_arr[ARRAY_NORMAL] = Array();
	Array tree = tr["tree"];
	Array leaf = tr["leaf"];
	Array parent = tr["parent"];
	int i, j, prevrow, thisrow, point;
	float x, y, z, u, v, currradius;

	Vector<Vector3> points;
	Vector<Vector3> normals;
	Vector<float> tangents;
	Vector<Vector2> uvs;
	Vector<int> indices;
	point = 0;
	//Generate a Cylinder for the trunk for each transform in the tree array
	thisrow = 0;
	prevrow = 0;
	for (int cr = 0; cr < tree.size(); cr++) {
		Transform3D t = tree[cr];
		Transform3D n = parent[cr];
		Transform3D par = n * IDENTITY.translated_local(Vector3(0.0, 0.5, 0.0));

		for (j = 0; j <= (rings + 1); j++) {
			v = j;
			v /= (rings + 1);

			currradius = radius;
			y = 1.0 * v;
			y = (0.1) - y * 0.5;

			for (i = 0; i <= radial_segments; i++) {
				u = i;
				u /= radial_segments;

				x = sin(u * (Math_PI * 2.0));
				z = cos(u * (Math_PI * 2.0));

				Vector3 pointa = Vector3(x * currradius, y, z * currradius);
				Transform3D cur = t.interpolate_with(par, v);
				Vector3 p = ((cur)*Transform3D(Basis(1, 0, 0, 0, 1, 0, 0, 0, 1), pointa)).origin;
				Vector3 c = ((cur)*Transform3D(Basis(1, 0, 0, 0, 1, 0, 0, 0, 1), Vector3(0, y, 0))).origin;
				Vector3 nn = (p - (c)).normalized();
				Vector3 tangent = cur.basis[1].normalized().cross(nn).normalized();
				points.push_back(p);
				normals.push_back(nn);
				ADD_TANGENT(tangent.x, tangent.y, tangent.z, 1.0)
				uvs.push_back(Vector2(u, v));
				point++;

				if (i > 0 && j > 0) {
					indices.push_back(prevrow + i - 1);
					indices.push_back(prevrow + i);
					indices.push_back(thisrow + i - 1);

					indices.push_back(prevrow + i);
					indices.push_back(thisrow + i);
					indices.push_back(thisrow + i - 1);
				};
			};

			prevrow = thisrow;
			thisrow = point;
		};

		/* add top
		if (top_radius > 0.0) {
			y = 1.0 * 0.5;

			thisrow = point;
			points.push_back(((t*Transform(Basis(1, 0, 0, 0, 1, 0, 0, 0, 1 ),Vector3(0.0, y, 0.0)))).origin);

			normals.push_back(dir);
			ADD_TANGENT(1.0, 0.0, 0.0, 1.0)
			uvs.push_back(Vector2(0.5,1.0)*Vector2(0.25, 0.75));
			point++;

			for (i = 0; i <= radial_segments; i++) {
				float r = i;
				r /= radial_segments;

				x = sin(r * (Math_PI * 2.0));
				z = cos(r * (Math_PI * 2.0));

				u = ((x + 1.0) * 0.25);
				v = 0.5 + ((z + 1.0) * 0.25);

				Vector3 p = (t*Transform(Basis(1, 0, 0, 0, 1, 0, 0, 0, 1 ),Vector3(x * top_radius, y, z * top_radius))).origin;
				points.push_back(p);
				ADD_TANGENT(1.0, 0.0, 0.0, 1.0)
				uvs.push_back(Vector2(0.5,1.0)*Vector2(u, v));
				point++;

				if (i > 0) {
					indices.push_back(thisrow);
					indices.push_back(point - 1);
					indices.push_back(point - 2);
				};
				normals.push_back(dir);
			};
		};

		// add bottom
		if (bottom_radius > 0.0) {
			y = 1.0 * -0.5;

			thisrow = point;
			points.push_back((t*Transform(Basis(1, 0, 0, 0, 1, 0, 0, 0, 1 ),Vector3(0.0, y, 0.0))).origin);
			normals.push_back(-dir);
			ADD_TANGENT(1.0, 0.0, 0.0, 1.0)
			uvs.push_back(Vector2(0.5,1.0)*Vector2(0.75, 0.75));
			point++;

			for (i = 0; i <= radial_segments; i++) {
				float r = i;
				r /= radial_segments;

				x = sin(r * (Math_PI * 2.0));
				z = cos(r * (Math_PI * 2.0));

				u = 0.5 + ((x + 1.0) * 0.25);
				v = 1.0 - ((z + 1.0) * 0.25);

				Vector3 p = (t*Transform(Basis(1, 0, 0, 0, 1, 0, 0, 0, 1 ),Vector3(x * bottom_radius, y, z * bottom_radius))) .origin;
				points.push_back(p);

				ADD_TANGENT(1.0, 0.0, 0.0, 1.0)
				uvs.push_back(Vector2(0.5,1.0)*Vector2(u, v));
				point++;

				if (i > 0) {
					indices.push_back(thisrow);
					indices.push_back(point - 2);
					indices.push_back(point - 1);
				};
				normals.push_back(-dir);
			};
		};*/
		thisrow = point;
	}
	// for(int lr = 0; lr < leaf.size(); lr++){
	// 	Transform t = leaf[lr];
	// 	Vector3 a = (t*Transform(Basis(1, 0, 0, 0, 1, 0, 0, 0, 1 ),Vector3(0.0, 1.0, 0.5)*leafScale)).origin;
	// 	Vector3 b = (t*Transform(Basis(1, 0, 0, 0, 1, 0, 0, 0, 1 ),Vector3(0.0, 1.0, -0.5)*leafScale)).origin;
	// 	Vector3 c = (t*Transform(Basis(1, 0, 0, 0, 1, 0, 0, 0, 1 ),Vector3(0.0, 0.0, 0.5)*leafScale)).origin;
	// 	Vector3 d = (t*Transform(Basis(1, 0, 0, 0, 1, 0, 0, 0, 1 ),Vector3(0.0, 0.0, -0.5)*leafScale)).origin;
	// 	Vector3 n = (t*Transform(Basis(1, 0, 0, 0, 1, 0, 0, 0, 1 ),Vector3(-1.0, 0.0, 0.0))).origin - t.origin;
	// 	points.push_back(a);
	// 	points.push_back(b);
	// 	points.push_back(c);
	// 	points.push_back(c);
	// 	points.push_back(b);
	// 	points.push_back(d);
	// 	normals.push_back(-n);
	// 	normals.push_back(-n);
	// 	normals.push_back(-n);
	// 	normals.push_back(-n);
	// 	normals.push_back(-n);
	// 	normals.push_back(-n);
	// 	ADD_TANGENT(1.0, 0.0, 0.0, 1.0);
	// 	ADD_TANGENT(1.0, 0.0, 0.0, 1.0);
	// 	ADD_TANGENT(1.0, 0.0, 0.0, 1.0);
	// 	ADD_TANGENT(1.0, 0.0, 0.0, 1.0);
	// 	ADD_TANGENT(1.0, 0.0, 0.0, 1.0);
	// 	ADD_TANGENT(1.0, 0.0, 0.0, 1.0);
	// 	uvs.push_back(split+Vector2(0.5,1.0)*Vector2(0.0, 0.0));
	// 	uvs.push_back(split+Vector2(0.5,1.0)*Vector2(1.0, 0.0));
	// 	uvs.push_back(split+Vector2(0.5,1.0)*Vector2(0.0, 1.0));
	// 	uvs.push_back(split+Vector2(0.5,1.0)*Vector2(0.0, 1.0));
	// 	uvs.push_back(split+Vector2(0.5,1.0)*Vector2(1.0, 0.0));
	// 	uvs.push_back(split+Vector2(0.5,1.0)*Vector2(1.0, 1.0));

	// 	indices.push_back(point);
	// 	indices.push_back(point+1);
	// 	indices.push_back(point+2);
	// 	indices.push_back(point+3);
	// 	indices.push_back(point+4);
	// 	indices.push_back(point+5);
	// 	point += 6;
	// 	thisrow = point;
	//}
	p_arr[ARRAY_VERTEX] = points;
	p_arr[ARRAY_NORMAL] = normals;
	p_arr[ARRAY_TANGENT] = tangents;
	p_arr[ARRAY_TEX_UV] = uvs;
	p_arr[ARRAY_INDEX] = indices;
}
