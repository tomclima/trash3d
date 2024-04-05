#include <math.h>

struct VECTOR3D
{
	float values[3];
};


VECTOR3D set_vector(float array[3])
{
    VECTOR3D defined_vector;

    for(int i = 0; i < 3; i++)
    {
        defined_vector.values[i] = array[i];
    };

    return defined_vector;
}

VECTOR3D vec_sum(VECTOR3D vec1, VECTOR3D vec2)
{
    VECTOR3D resulting_vec;
    float result;

    for(int i = 0; i < 3; i++)
    {
        result = vec1.values[i] + vec2.values[i];
        resulting_vec.values[i] = result;
    };

    return resulting_vec;
};

VECTOR3D vec_MultbyScalar(VECTOR3D vec1, float scalar)
{
    VECTOR3D resulting_vec;
    float result;

    for(int i = 0; i < 3; i++)
    {
        result = vec1.values[i] * scalar;
        resulting_vec.values[i] = result;
    };

    return resulting_vec;
};

float vec_DotProduct(VECTOR3D vec1, VECTOR3D vec2)
{
    float result = 0;

    for(int i = 0; i < 3; i++)
    {
        result += vec1.values[i] * vec2.values[i];
    };

    return result;
};

float vec_norm(VECTOR3D vector)
{
    //teste
    float norm;
    float squared_norm = vec_DotProduct(vector, vector);
    norm = sqrt(squared_norm);
    return norm;
};

float vec_cosine(VECTOR3D vec1, VECTOR3D vec2)
{
    // angle formula <vec1, vec2>/|vec1|*|vec2|
    float angle;
    float dot_product = vec_DotProduct(vec1, vec2);
    float vec1_norm = vec_norm(vec1);
    float vec2_norm = vec_norm(vec2);
    
    angle = dot_product/(vec1_norm * vec2_norm);
    return angle;
}


struct MATRIX3D
{
    VECTOR3D lines[3];
};

MATRIX3D set_matrix(VECTOR3D vec1, VECTOR3D vec2, VECTOR3D vec3)
{
    MATRIX3D tmatrix;
    tmatrix.lines[0] =  vec1;
    tmatrix.lines[1] = vec2;
    tmatrix.lines[2] = vec3;

    return tmatrix;
};

MATRIX3D mat_transpose(MATRIX3D matrix)
{
    MATRIX3D transposed_matrix;

    for(int i =0; i< 3; i++)
    {
        for(int j = 0; j< 3; j++)
        {
            transposed_matrix.lines[i].values[j] = matrix.lines[j].values[i];
        }
    }

    return transposed_matrix;
}   

MATRIX3D get_identity_matrix()
{
    MATRIX3D identity_matrix;

    float line1[3] = {1,0,0};
    float line2[3] = {0,1,0};
    float line3[3] = {0,0,1};

    VECTOR3D lines[3] = {set_vector(line1), set_vector(line2), set_vector(line3)};

    identity_matrix = set_matrix(lines[0], lines[1], lines[2]);

    return identity_matrix;
}

MATRIX3D rotate_y(float angle)
{
    MATRIX3D rotation_matrix;

    float line1[3] = {cos(angle), 0, sin(angle)};
    float line2[3] = {0, 1, 0};
    float line3[3] = {-sin(angle), 0, cos(angle)};

    VECTOR3D lines[3] = {set_vector(line1), set_vector(line2), set_vector(line3)};

    rotation_matrix = set_matrix(lines[0], lines[1], lines[2]);

    return rotation_matrix;
}

VECTOR3D vec_linear_transform(MATRIX3D tmatrix, VECTOR3D vector)
{
    VECTOR3D transformed_vector;

    for(int i = 0; i < 3; i++)
    {
        float value_n = 0;
        for(int j = 0; j < 3; j++)
        {
            value_n += tmatrix.lines[i].values[j]*vector.values[j];
        };
        transformed_vector.values[i] = value_n;
    };
    return transformed_vector;
};