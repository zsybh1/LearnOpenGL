#pragma once

#include <global.h>
#include <Object/IndexObject.h>
#include <OBJ_Loader.h>
#include <Shader.h>
#include <Texture/TextureLoader.h>

namespace psi{

class Mesh {
public:
    std::vector<Ptr<IndexObject>> parts;
    std::vector<objl::Material> materials;
    Hashmap<std::string, Ptr<Texture>> textures;

    Mesh() = default;
    Mesh(const std::string& path) {
        SetupMesh(path);
    }
    void LoadFrom(const std::string& path) {
        parts.clear();
        materials.clear();
        textures.clear();
        SetupMesh(path);
    }

    void Draw(Shader& shader) {
        for (size_t i = 0; i < parts.size(); ++i) {
            auto& mat = materials[i];
            shader.Use();
            shader.SetVec3f("Ka", trans(mat.Ka))
                .SetVec3f("Kd", trans(mat.Kd))
                .SetVec3f("Ks", trans(mat.Ks))
                .SetFloat("Ns", mat.Ns)
                .SetFloat("Ni", mat.Ni)
                .SetFloat("d", mat.d)
                .SetInt("illum", mat.illum);
            if (mat.map_Ka != "") {
                shader.SetInt("map_Ka", 0).SetInt("UseKaMap", true);
                textures[mat.map_Ka]->Use(0);
            }
            else {
                shader.SetInt("UseKaMap", false);
            }
            if (mat.map_Kd != "") {
                shader.SetInt("map_Kd", 1).SetInt("UseKdMap", true);
                textures[mat.map_Kd]->Use(1);
            }
            else {
                shader.SetInt("UseKdMap", false);
            }
            if (mat.map_Ks != "") {
                shader.SetInt("map_Ks", 2).SetInt("UseKsMap", true);
                textures[mat.map_Ks]->Use(2);
            }
            else {
                shader.SetInt("UseKsMap", false);
            }
            if (mat.map_Ns != "") {
                shader.SetInt("map_Ns", 3).SetInt("UseNsMap", true);
                textures[mat.map_Ns]->Use(3);
            }
            else {
                shader.SetInt("UseNsMap", false);
            }
            if (mat.map_d != "") {
                shader.SetInt("map_d", 4).SetInt("UseDMap", true);
                textures[mat.map_d]->Use(4);
            }
            else {
                shader.SetInt("UseBumpMap", false);
            }
            if (mat.map_bump != "") {
                shader.SetInt("map_bump", 5).SetInt("UseBumpMap", true);
                textures[mat.map_bump]->Use(5);
            }
            else {
                shader.SetInt("UseBumpMap", false);
            }
            parts[i]->Draw();
        }
    }

private:
    std::string pathPrefix;
    void SetupMesh(const std::string& path) {
        pathPrefix = GetPathPrefix(path);
        objl::Loader loader;
        if (loader.LoadFile(path)) {
            for (auto& mesh : loader.LoadedMeshes) {
                parts.push_back(New<IndexObject>((float*)mesh.Vertices.data(), mesh.Vertices.size() * sizeof(objl::Vertex),
                    std::initializer_list<int>{3, 3, 2}, (unsigned*)mesh.Indices.data(), mesh.Indices.size() * sizeof(unsigned)));
                materials.push_back(mesh.MeshMaterial);
                LoadTexture(mesh.MeshMaterial.map_Ka);
                LoadTexture(mesh.MeshMaterial.map_Kd);
                LoadTexture(mesh.MeshMaterial.map_Ks);
                LoadTexture(mesh.MeshMaterial.map_Ns);
                LoadTexture(mesh.MeshMaterial.map_d);
                LoadTexture(mesh.MeshMaterial.map_bump);
            }
        }
        else {
            std::cout << "Failed to load model " << path << std::endl;
        }
    }
    Vec3f trans(objl::Vector3 vec) {
        return Vec3f(vec.X, vec.Y, vec.Z);
    }
    void LoadTexture(const std::string& name) {
        if (name != "") {
            if (textures.find(name) == textures.cend()) {
                textures[name] = TextureLoader::LoadTexture(pathPrefix + name, false);
            }
        }
    }
    static std::string GetPathPrefix(const std::string& path) {
        int index1 = path.find_last_of('/');
        int index2 = path.find_last_of('\\');
        int index = std::max(index1, index2) + 1;
        return path.substr(0, index);
    }
};

}