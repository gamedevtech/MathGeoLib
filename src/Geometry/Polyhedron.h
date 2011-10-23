/* Copyright 2011 Jukka Jyl�nki

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

/** @file Polyhedron.h
	@author Jukka Jyl�nki
	@brief The Polyhedron geometry object. */
#pragma once

#include "Math/MathFwd.h"
#include "Math/float3.h"

//#ifdef MATH_ENABLE_STL_SUPPORT
#include <vector>
//#endif

MATH_BEGIN_NAMESPACE

/// Represents a three-dimensional closed geometric solid defined by flat polygonal faces.
class Polyhedron
{
public:
	/// Stores a list of indices of a single face of a Polyhedron.
	struct Face
	{
		/// Specifies the indices of the corner vertices of this polyhedron.
		/// Indices point to the polyhedron vertex array.
		/// The face vertices should all lie on the same plane.
		/// The positive direction of the plane (the direction the face outwards normal points to) 
		/// is the one where the vertices are wound in counter-clockwise order.
		std::vector<int> v;
	};

	/// Specifies the vertices of this polyhedron.
	std::vector<float3> v;

	/// Specifies the individual faces of this polyhedron.  [similarOverload: v]
	/** Each face is described by a list of indices to the vertex array. The indices define a 
		simple polygon in counter-clockwise winding order. */
	std::vector<Face> f;

	/// The default constructor creates a null polyhedron.
	/** A null polyhedron has 0 vertices and 0 faces.
		@see IsNull(). */
	Polyhedron() {}

	/// Returns the number of vertices in this polyhedron.
	/** The running time of this function is O(1).
		@see NumFaces(), NumEdges(), EulerFormulaHolds(). */
	int NumVertices() const { return v.size(); }

	/// Returns the number of faces in this polyhedron.
	/** The running time of this function is O(1).
		@see NumVertices(), NumEdges(), EulerFormulaHolds(), FacePolygon(), FacePlane(). */
	int NumFaces() const { return f.size(); }

	/// Returns the number of (unique) edges in this polyhedron.
	/** This function will enumerate through all faces of this polyhedron to compute the number of unique edges.
		The running time is linear to the number of faces and vertices in this Polyhedron.
		@see NumVertices(), NumFaces(), EulerFormulaHolds(), Edge(), Edges(), EdgeIndices(). */
	int NumEdges() const;

	/// Returns the <i>i</i>th vertex of this polyhedron.
	/** @param vertexIndex The vertex to get, in the range [0, NumVertices()-1].
		@see NumVertices(). */
	float3 Vertex(int vertexIndex) const;

	/// Returns the <i>i</i>th edge of this polyhedron.
	/** Performance warning: Use this function only if you are interested in a single edge of this Polyhedron.
		This function calls the Edges() member function to receive a list of all the edges, so has
		a complexity of O(|V|log|V|), where |V| is the number of vertices in the polyhedron.
		@param edgeIndex The index of the edge to get, in the range [0, NumEdges()-1].
		@see NumEdges(), Edges(), EdgeIndices(). */
	LineSegment Edge(int edgeIndex) const;

	/// Returns all the (unique) edges of this polyhedron.
	/** Has complexity of O(|V|log|V|), where |V| is the number of vertices in the polyhedron. 
		@todo Support this in linear time.
		@see NumEdges(), Edge(), EdgeIndices(). */
	std::vector<LineSegment> Edges() const;

	/// Returns all the (unique) edges of this polyhedron, as indices to the polyhedron vertex array.
	/** Has complexity of O(|V|log|V|), where |V| is the number of vertices in the polyhedron. 
		@todo Support this in linear time.
		@see NumEdges(), Edge(), Edges(). */
	std::vector<std::pair<int, int> > EdgeIndices() const;

	/// Returns a polygon representing the given face.
	/** The winding order of the polygon will be the same as in the input. The normal of the polygon
		points outwards from this polyhedron, i.e. towards the space that is not part of the polyhedron.
		This function constructs a new Polygon object, so it has a time and space complexity of
		O(|V|), where |V| is the number of vertices in this polyhedron. 
		@param faceIndex The index of the face to get, in the range [0, NumFaces()-1].
		@see NumFaces(), FacePlane(). */
	Polygon FacePolygon(int faceIndex) const;

	/// Returns the plane of the given polyhedron face.
	/** The normal of the plane points outwards from this polyhedron, i.e. towards the space that 
		is not part of the polyhedron.
		This function assumes that the given face of the polyhedron is planar, as should be for all
		well-formed polyhedron.
		@param faceIndex The index of the face to get, in the range [0, NumFaces()-1].
		@see NumFaces(), FacePolygon(). */
	Plane FacePlane(int faceIndex) const;

	/// Returns the index of the vertex of this polyhedron that reaches farthest in the given direction.
	/** @param dir The direction vector to query for. This vector can be unnormalized.
		@return The supporting point of this polyhedron that reaches farthest in the given direction.
			The supporting point for a given direction is not necessarily unique, but this function
			will always return one of the vertices of this polyhedron.
		@see v, NumVertices(), Vertex(). */
	int ExtremeVertex(const float3 &dir) const;
	// float3 SupportingPoint(const float3 &dir) const;
	// bool IsSupportingPlane(const Plane &plane) const;

	/// Returns the arithmetic mean of all the corner vertices.
	/** @bug This is not the proper centroid of the polyhedron! */
	/** @see SurfaceArea(), Volume(). */
	float3 Centroid() const;

	/// Computes the total surface area of the faces of this polyhedron.
	/** @see Centroid(), Volume(). */
	float SurfaceArea() const;

	/// Computes the internal volume of this polyhedron.
	/** @see Centroid(), SurfaceArea(). */
	float Volume() const;

	/// Returns the smallest AABB that encloses this polyhedron.
	/// @todo Add MinimalEnclosingSphere() and MinimalEnclosingOBB().
	AABB MinimalEnclosingAABB() const;

	/// Tests if the faces in this polyhedron refer to valid existing vertices.
	/** This function performs sanity checks on the face indices array.
		1) Each vertex index for each face must be in the range [0, NumVertices()-1], i.e. refer to a vertex 
		   that exists in the array.
		2) Each face must contain at least three vertices. If a face contains two or one vertex, it is
		   degenerate.
		3) Each face may refer to a vertex at most once.
		@return True if the abovementioned conditions hold. Note that this does not guarantee that the 
			polyhedron is completely well-formed, but if false is returned, the polyhedron is definitely
			ill-formed.
		@see IsNull(), IsClosed(), IsConvex(). */
	bool FaceIndicesValid() const; 

	/// Returns true if this polyhedron has 0 vertices and 0 faces.
	/** @see FaceIndicesValid(), IsClosed(), IsConvex(). */
	bool IsNull() const { return v.size() == 0 && f.size() == 0; }

	/// Returns true if this polyhedron is closed and does not have any gaps.
	/** \note This function performs a quick check, which might not be complete.
		@see FaceIndicesValid(), IsClosed(), IsConvex(). */
	bool IsClosed() const;

	// Returns true if this polyhedron forms a single connected solid volume.
//	bool IsConnected() const;

	/// Returns true if this polyhedron is convex.
	/** The running time is O(F*V) ~ O(V^2).
		@see FaceIndicesValid(), IsClosed(), IsConvex().*/
	bool IsConvex() const;

	/// Returns true if the Euler formula (V + F - E == 2) holds for this Polyhedron.
	/** @see NumVertices(), NumEdges(), NumFaces(). */
	bool EulerFormulaHolds() const;

	/// Clips the line/ray/line segment specified by L(t) = ptA + t * dir, tFirst <= t <= tLast, 
	/// inside this <b>convex</b> polyhedron.
	/** The implementation of this function is adapted from Christer Ericson's Real-time Collision Detection, p. 199.
		@param ptA The first endpoint of the line segment.
		@param dir The direction of the line segment. This member can be unnormalized.
		@param tFirst [in, out] As input, takes the parametric distance along the line segment which
			specifies the starting point of the line segment. As output, the starting point of the line segment
			after the clipping operation is returned here.
		@param tLast [in, out] As input, takes the parametric distance along the line segment which
			specifies the ending point of the line segment. As output, the endingpoint of the line segment
			after the clipping operation is returned here.
		@note To clip a line, pass in tFirst=-FLOAT_INF, tLast=FLOAT_INF. To clip a ray, pass in tFirst=0 and tLast = FLOAT_INF.
			To clip a line segment, pass in tFirst=0, tLast=1, and an unnormalized dir = lineSegment.b-lineSegment.a.
		@return True if the outputted range [tFirst, tLast] did not become degenerate, and these two variables contain
			valid data. If false, the whole line segment was clipped away (it was completely outside this polyhedron).
		@see FLOAT_INF. */
	bool ClipLineSegmentToConvexPolyhedron(const float3 &ptA, const float3 &dir, float &tFirst, float &tLast) const;

	/// Tests if the given object is fully contained inside this polyhedron.
	/** This function treats this polyhedron as a non-convex object. If you know this polyhedron
		to be convex, you can use the faster ContainsConvex() function.
		@see ContainsConvex(), ClosestPoint(), ClosestPointConvex(), Distance(), Intersects(), IntersectsConvex().
		@todo Add Contains(Circle/Disc/Sphere/Capsule). */
	bool Contains(const float3 &point) const;
	bool Contains(const LineSegment &lineSegment) const;
	bool Contains(const Triangle &triangle) const;
	bool Contains(const Polygon &polygon) const;
	bool Contains(const AABB &aabb) const;
	bool Contains(const OBB &obb) const;
	bool Contains(const Frustum &frustum) const;
	bool Contains(const Polyhedron &polyhedron) const;

	/// Tests if the given object is fully contained inside this <b>convex</b> polyhedron.
	/** This function behaves exactly like Contains(), except this version of the containment test 
		assumes this polyhedron is convex, and uses a faster method of testing containment.
		@see Contains(), ClosestPoint(), ClosestPointConvex(), Distance(), Intersects(), IntersectsConvex().
		@todo Add ContainsConvex(Polygon/AABB/OBB/Frustum/Polyhedron/Circle/Disc/Sphere/Capsule). */
	bool ContainsConvex(const float3 &point) const;
	bool ContainsConvex(const LineSegment &lineSegment) const;
	bool ContainsConvex(const Triangle &triangle) const;

	/// Computes the closest point on this polyhedron to the given object.
	/** If the other object intersects this polyhedron, this function will return an arbitrary point inside
		the region of intersection.
		@see Contains(), ContainsConvex(), ClosestPointConvex(), Distance(), Intersects(), IntersectsConvex().
		@todo Add ClosestPoint(Line/Ray/Plane/Triangle/Polygon/Circle/Disc/AABB/OBB/Sphere/Capsule/Frustum/Polyhedron). */
	float3 ClosestPoint(const float3 &point) const;
	float3 ClosestPoint(const LineSegment &lineSegment) const;
	/** @param lineSegmentPt [out] If specified, returns the closest point on the line segment to this
		polyhedron. This pointer may be null. */
	float3 ClosestPoint(const LineSegment &lineSegment, float3 *lineSegmentPt) const;

	/// Returns the closest point on this <b>convex</b> polyhedron to the given point.
	/** This function behaves exactly like ClosestPoint(), except this version of the test assumes
		this polyhedron is convex, and uses a faster method of finding the closest point.
		@see Contains(), ContainsConvex(), ClosestPoint(), Distance(), Intersects(), IntersectsConvex().
		@todo Add ClosestPointConvex(Line/LineSegment/Ray/Plane/Triangle/Polygon/Circle/Disc/AABB/OBB/Sphere/Capsule/Frustum/Polyhedron). */
	float3 ClosestPointConvex(const float3 &point) const;

	/// Returns the distance between this polyhedron and the given object.
	/** This function finds the nearest pair of points on this and the given object, and computes their distance.
		If the two objects intersect, or one object is contained inside the other, the returned distance is zero.
		@see Contains(), ContainsConvex(), ClosestPoint(), ClosestPointConvex(), Intersects(), IntersectsConvex().
		@todo Add Distance(Line/LineSegment/Ray/Plane/Triangle/Polygon/Circle/Disc/AABB/OBB/Sphere/Capsule/Frustum/Polyhedron). */
	float Distance(const float3 &point) const;

	/// Tests whether this polyhedron and the given object intersect.
	/** Both objects are treated as "solid", meaning that if one of the objects is fully contained inside 
		another, this function still returns true. (e.g. in case a line segment is contained inside this polyhedron, 
		or this polyhedron is contained inside a sphere, etc.)
		@return True if an intersection occurs or one of the objects is contained inside the other, false otherwise.
		@see Contains(), ContainsConvex(), ClosestPoint(), ClosestPointConvex(), Distance(), IntersectsConvex().
		@todo Add Intersects(Circle/Disc). */
	bool Intersects(const LineSegment &lineSegment) const;
	bool Intersects(const Line &line) const;
	bool Intersects(const Ray &ray) const;
	bool Intersects(const Plane &plane) const;
	bool Intersects(const Polyhedron &polyhedron) const;
	bool Intersects(const AABB &aabb) const;
	bool Intersects(const OBB &obb) const;
	bool Intersects(const Triangle &triangle) const;
	bool Intersects(const Polygon &polygon) const;
	bool Intersects(const Frustum &frustum) const;
	bool Intersects(const Sphere &sphere) const;
	bool Intersects(const Capsule &capsule) const;

	/// Tests whether this <b>convex</b> polyhedron and the given object intersect.
	/** This function is exactly like Intersects(), but this version assumes that this polyhedron is convex,
		and uses a faster method of testing the intersection.
		@return True if an intersection occurs or one of the objects is contained inside the other, false otherwise.
		@see Contains(), ContainsConvex(), ClosestPoint(), ClosestPointConvex(), Distance(), Intersects().
		@todo Add Intersects(Circle/Disc). */
	bool IntersectsConvex(const Line &line) const;
	bool IntersectsConvex(const Ray &ray) const;
	bool IntersectsConvex(const LineSegment &lineSegment) const;
};

MATH_END_NAMESPACE
