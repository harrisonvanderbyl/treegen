# treegen

A GDExtension for Godot 4 that procedurally generates tree and flower meshes.

## Classes

- **`TreeGenerator`** — A `Mesh` resource that builds a branching tree. The
  trunk is recursively split into child branches and skinned as a cylinder
  mesh. Parameters are grouped in the inspector:
  - **Branches** — `branch_depth`, `branch_distribution`, `branch_auto_split`,
    `branch_splits`, `branch_rotation`, `branch_radius_decay`,
    `branch_length_decay`
  - **Trunk** — `trunk_radius`, `trunk_height`, `trunk_width`,
    `trunk_leaf_scale`
  - **Mesh** — `mesh_radial_segments`, `mesh_rings`
  - The `mesh_a` property (inherited from `DualMesh`) takes a
    `FlowerGenerator` whose petals are placed at every leaf transform.

- **`FlowerGenerator`** — A `PrimitiveMesh` that arranges petals around a
  point. Petal shape is driven by two `Curve` resources (`petal_curve_left`
  for width, `petal_curve_top` for height). All petal parameters are grouped
  under the **Petal** prefix.

- **`DualMesh`** — Abstract base for `TreeGenerator`. It owns a `RenderingServer`
  mesh with up to two surfaces: the primary mesh (produced by the subclass's
  `_create_mesh_array`) and an optional secondary mesh produced by the
  `FlowerGenerator` assigned to `mesh_a`.

## Building

This extension depends on [godot-cpp](https://github.com/godotengine/godot-cpp),
which is expected as a git submodule in the `godot-cpp/` folder:

```bash
git submodule add https://github.com/godotengine/godot-cpp godot-cpp
cd godot-cpp
git checkout <godot-version> # e.g. 4.5
git submodule update --init --recursive
```

Then build with SCons:

```bash
scons platform=linux target=template_release
```

The shared library is written to `bin/<platform>/` and copied into
`project/bin/<platform>/` so the `.gdextension` file can find it.

## Using in a project

Copy `project/bin/treegen.gdextension` (and the built libraries next to it)
into your Godot project. The `TreeGenerator` and `FlowerGenerator` types will
then appear in the "Create New Resource" / mesh type dialogs.
