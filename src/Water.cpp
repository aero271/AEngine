#include "Water.hpp"

#include <cstdlib>
#include <array>
#include <cmath>
#include <raymath.h>
#include <iostream>
#include "rlights.h"
#include <boost/format.hpp>
#include <stdexcept>
using boost::format;

#include "definitions.hpp"
#include "Item.hpp"

Texture2D Water::blackMap = { 0 };

void printVec3(Vector3 vec)
{
    std::cout << vec.x << ", "
        << vec.y << ", "
        << vec.z << "\n";
        
}

Water::Water(Vector2 position_, float cellSize_, int size_) 
    : size(size_), position(position_), cellSize(cellSize_) 
{
    chunkSize = cellSize * (size - 1);
    
    mesh.triangleCount = (size - 1) * (size - 1) * 2;
    mesh.vertexCount = size * size;

    mesh.vertices = (float*)malloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.texcoords = (float*)malloc(mesh.vertexCount * 2 * sizeof(float));
    mesh.indices = (unsigned short*)malloc(mesh.triangleCount * 3 * sizeof(unsigned short));
    if (!mesh.vertices || !mesh.texcoords || !mesh.indices)
    {
        throw std::runtime_error("ERROR: Water mesh memory allocation failed");
    }

    for (int z = 0; z < size; z++) {
        for (int x = 0; x < size; x++) {
            //this is because its mapping the 2d cords onto a 1d array (and each vertex has 3 data points, thus *3)
            int idx = (z * size + x) * 3;
            
            float worldX = position.x * chunkSize + x * cellSize;
            float worldZ = position.y * chunkSize + z * cellSize;


            mesh.vertices[idx] = worldX;
            mesh.vertices[idx + 1] = 0.0f;
            mesh.vertices[idx + 2] = worldZ;
        }
    }

    int idx = 0;
    for (int z = 0; z < size - 1; z++) {
        for (int x = 0; x < size - 1; x++) {
            int topLeft = z * size + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * size + x;
            int bottomRight = bottomLeft + 1;
            
            // First triangle
            mesh.indices[idx] = topLeft;
            mesh.indices[idx + 1] = bottomLeft;
            mesh.indices[idx + 2] = topRight;
            
            // Second triangle
            mesh.indices[idx + 3] = topRight;
            mesh.indices[idx + 4] = bottomLeft;
            mesh.indices[idx + 5] = bottomRight;
            
            idx += 6;  
        }
    }

    UploadMesh(&mesh, true);
}

/// @brief depricated, use GlobalWater::updateWater()
void Water::render()
{
    float time = GetTime();
    
    // Update mesh vertices based on wave heights
    for (int z = 0; z < size; z++) {
        for (int x = 0; x < size; x++) {
            //this is because its mapping the 2d cords onto a 1d array (and each vertex has 3 data points, thus *3)
            int idx = (z * size + x) * 3;
            
            float worldX = position.x * chunkSize + x * cellSize;
            float worldZ = position.y * chunkSize + z * cellSize;

            mesh.vertices[idx] = worldX;
            mesh.vertices[idx + 2] = worldZ;

            //centers square around origin and scales based on res (cell size)
            
            float height = waveHeight(worldX, worldZ, time);
            // std::cout << format("Vertex %1%s cords: %2%, %3%, %4%\n") % idx % worldX % height % worldZ;
            // Update vertex position
            mesh.vertices[idx + 1] = height;

            //reset mesh tangents from last frame to prepare them for tangent calculations later
            mesh.tangents[idx] = 0.0f;
            mesh.tangents[idx + 1] = 0.0f;
            mesh.tangents[idx + 2] = 0.0f;
        }
    }
    
    for (int z = 0; z < size; z++) {
        for (int x = 0; x < size; x++)
        {
            int vertexIdx = (z * size + x) * 3;
            Vector3 vertex = { mesh.vertices[vertexIdx], mesh.vertices[vertexIdx + 1], mesh.vertices[vertexIdx + 2] };

            int adjXIdx;
            if (x == (size - 1)) adjXIdx = (z * size + x - 1) * 3;
            else adjXIdx = (z * size + x + 1) * 3;
            Vector3 adjX = { mesh.vertices[adjXIdx], mesh.vertices[adjXIdx + 1], mesh.vertices[adjXIdx + 2] };

            int adjZIdx;
            if (z == (size - 1)) adjZIdx = ((z - 1) * size + x) * 3;
            else adjZIdx = ((z + 1) * size + x) * 3; 
            Vector3 adjZ = { mesh.vertices[adjZIdx], mesh.vertices[adjZIdx + 1], mesh.vertices[adjZIdx + 2] };
            //std::cout << format("adjx: %1%, adjz: %2%\n") % adjXIdx % adjZIdx;

            Vector3 unitAdjX = Vector3Normalize(Vector3Subtract(adjX, vertex));
            Vector3 unitAdjZ = Vector3Normalize(Vector3Subtract(adjZ, vertex));

            Vector3 normal = Vector3Normalize(Vector3CrossProduct(unitAdjX, unitAdjZ));

            int negation = (x == (size - 1) || z == (size - 1)) ? 1 : -1;

            mesh.normals[vertexIdx] = negation * normal.x;
            mesh.normals[vertexIdx + 1] = negation * normal.y;
            mesh.normals[vertexIdx + 2] = negation * normal.z;
        }
    }
    
    /*
    for (int i = 0; i < mesh.triangleCount; i++)
    {
        int idx = i * 3;

        int vertIdx1 = mesh.indices[idx];
        int vertIdx2 = mesh.indices[idx + 1];
        int vertIdx3 = mesh.indices[idx + 2];

        Vector3 vertex1 = { mesh.vertices[(vertIdx1 * 3)], mesh.vertices[(vertIdx1 * 3) + 1], mesh.vertices[(vertIdx1 * 3) + 2] };
        Vector3 vertex2 = { mesh.vertices[(vertIdx2 * 3)], mesh.vertices[(vertIdx2 * 3) + 1], mesh.vertices[(vertIdx2 * 3) + 2] };
        Vector3 vertex3 = { mesh.vertices[(vertIdx3 * 3)], mesh.vertices[(vertIdx3 * 3) + 1], mesh.vertices[(vertIdx3 * 3) + 2] };
        Vector3 edge1 = Vector3Subtract(vertex2, vertex1);
        Vector3 edge2 = Vector3Subtract(vertex3, vertex1);

        Vector2 uv1 = { mesh.texcoords[(vertIdx1 * 2)], mesh.texcoords[(vertIdx1 * 2) + 1] };
        Vector2 uv2 = { mesh.texcoords[(vertIdx2 * 2)], mesh.texcoords[(vertIdx2 * 2) + 1] };
        Vector2 uv3 = { mesh.texcoords[(vertIdx3 * 2)], mesh.texcoords[(vertIdx3 * 2) + 1] };
        float du1 = uv2.x - uv1.x;
        float du2 = uv3.x - uv1.x;
        float dv1 = uv2.y - uv1.y;
        float dv2 = uv3.y - uv1.y;

        if ((du1 * dv2 - du2 * dv1) == 0) throw std::runtime_error("ERROR: textcords are colinear");
        float denom = 1.0f / (du1 * dv2 - du2 * dv1);
        Vector3 tangent = Vector3Scale(Vector3Subtract(Vector3Scale(edge1, dv2), Vector3Scale(edge2, dv1)), denom);
        Vector3 nTangent = Vector3Normalize(tangent);

        mesh.tangents[(vertIdx1 * 3)] += nTangent.x;
        mesh.tangents[(vertIdx1 * 3) + 1] += nTangent.y;
        mesh.tangents[(vertIdx1 * 3) + 2] += nTangent.z;

        mesh.tangents[(vertIdx2 * 3)] += nTangent.x;
        mesh.tangents[(vertIdx2 * 3) + 1] += nTangent.y;
        mesh.tangents[(vertIdx2 * 3) + 2] += nTangent.z;

        mesh.tangents[(vertIdx3 * 3)] += nTangent.x;
        mesh.tangents[(vertIdx3 * 3) + 1] += nTangent.y;
        mesh.tangents[(vertIdx3 * 3) + 2] += nTangent.z;
    }
    //last pass to normalize cumulated tangents
    for (int idx = 0; idx < mesh.vertexCount; idx += 3)
    {
        Vector3 avgTangent = { mesh.tangents[idx], mesh.tangents[idx + 1], mesh.tangents[idx + 2] };
        Vector3 finalTangent = Vector3Normalize(avgTangent);

        mesh.tangents[(idx)] += finalTangent.x;
        mesh.tangents[(idx) + 1] += finalTangent.y;
        mesh.tangents[(idx) + 2] += finalTangent.z;
    }
    */

    UpdateMeshBuffer(mesh, 0, mesh.vertices, mesh.vertexCount * 3 * sizeof(float), 0);
    UpdateMeshBuffer(mesh, 2, mesh.normals, mesh.vertexCount * 3 * sizeof(float), 0);
    //UpdateMeshBuffer(mesh, 4, mesh.tangents, mesh.vertexCount * 3 * sizeof(float), 0);
    
}


float Water::waveHeight(float x, float z, float time)
{
    float height = 0.0f;

    /*
    for (int i = 0; i < 3; i++)
    {
        float dot = x * waves[i].dir.x + z * waves[i].dir.y;

        height += waves[i].amplitude * sinf(waves[i].frequency * dot + waves[i].phase * time);
    }
    */
    return height;
}

Water::~Water()
{
    free(mesh.vertices);
    free(mesh.texcoords);
    free(mesh.indices);
}