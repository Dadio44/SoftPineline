using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;

public class FBXToObj : MonoBehaviour
{
    public GameObject model;

    [ContextMenu(nameof(OutputObj))]
    public void OutputObj()
    {

        var meshFilters = model.GetComponentsInChildren<MeshFilter>();

        foreach (var mf in meshFilters)
        {
            WriteMeshWithTransform(mf.sharedMesh, mf.transform);
        }

    }

    private void WriteMeshWithTransform(Mesh mesh, Transform transform)
    {
        var vertices = mesh.vertices;
        var normals = mesh.normals;
        var uvs = mesh.uv;

        var subMeshNumber = mesh.subMeshCount;
        List<int> indices = new List<int>();

        for (int i = 0; i < subMeshNumber; i++)
        {
            indices.AddRange(mesh.GetIndices(i));
        }


        var path = Path.Combine(Application.dataPath, Path.GetFileNameWithoutExtension(mesh.name) + ".obj");

        using (BinaryWriter writer = new BinaryWriter(File.Open(path, FileMode.Create)))
        {
            writer.Write(vertices.Length);
            writer.Write(normals.Length);
            writer.Write(uvs.Length);
            writer.Write(indices.Count);

            foreach (var v in vertices)
            {
                var tv = transform.TransformPoint(v);
                writer.Write(tv.x);
                writer.Write(tv.y);
                writer.Write(tv.z);
            }

            foreach (var n in normals)
            {
                var tn = transform.TransformVector(n);
                writer.Write(n.x);
                writer.Write(n.y);
                writer.Write(n.z);
            }

            foreach (var v in uvs)
            {
                writer.Write(v.x);
                writer.Write(v.y);
            }

            foreach (var v in indices)
            {
                writer.Write(v);
            }
        }
    }
}
