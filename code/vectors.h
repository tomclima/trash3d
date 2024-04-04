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

VECTOR3D vec_linear_transform(MATRIX3D tmatrix, VECTOR3D vector)
{
    VECTOR3D transformed_vector;

    for(int i = 0; i < 3; i++)
    {
        int value_n = 0;
        for(int j = 0; j < 3; j++)
        {
            value_n += tmatrix.lines[i].values[j]*vector.values[j];
        };
        transformed_vector.values[i] = value_n;
    };
    return transformed_vector;
};