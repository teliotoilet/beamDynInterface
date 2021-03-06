/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "beamDynInterfacePointPatchVectorField.H"
#include "pointPatchFields.H"
#include "addToRunTimeSelectionTable.H"
#include "Time.H"
#include "polyMesh.H"
#include "displacementMotionSolver.H"

//#include "beamDyn.H" // BD namespace

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

beamDynInterfacePointPatchVectorField::
beamDynInterfacePointPatchVectorField
(
    const pointPatch& p,
    const DimensionedField<vector, pointMesh>& iF
)
:
    fixedValuePointPatchField<vector>(p, iF)
{
    Info<< "Created instance of beamDynInterfacePointPatchVectorField (2)" << endl;
}


beamDynInterfacePointPatchVectorField::
beamDynInterfacePointPatchVectorField
(
    const pointPatch& p,
    const DimensionedField<vector, pointMesh>& iF,
    const dictionary& dict
)
:
    fixedValuePointPatchField<vector>(p, iF, dict)
{
    Info<< "Created instance of beamDynInterfacePointPatchVectorField (3)" << endl;
    if (!dict.found("value"))
    {
        updateCoeffs();
    }
}


beamDynInterfacePointPatchVectorField::
beamDynInterfacePointPatchVectorField
(
    const beamDynInterfacePointPatchVectorField& ptf,
    const pointPatch& p,
    const DimensionedField<vector, pointMesh>& iF,
    const pointPatchFieldMapper& mapper
)
:
    fixedValuePointPatchField<vector>(ptf, p, iF, mapper)
{
    Info<< "Created instance of beamDynInterfacePointPatchVectorField (4)" << endl;
}


beamDynInterfacePointPatchVectorField::
beamDynInterfacePointPatchVectorField
(
    const beamDynInterfacePointPatchVectorField& ptf,
    const DimensionedField<vector, pointMesh>& iF
)
:
    fixedValuePointPatchField<vector>(ptf, iF)
{
    Info<< "Created instance of beamDynInterfacePointPatchVectorField (2,ptf)" << endl;
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void beamDynInterfacePointPatchVectorField::updateCoeffs()
{
    if (this->updated())
    {
        return;
    }

    //const labelList& meshPoints = patch().meshPoints(); // returns polyPatch_.meshPoints(), i.e. node IDs
    const pointField& localPoints = patch().localPoints(); // returns polyPatch_.localPoints(), i.e. node coords

    const polyMesh& mesh = this->dimensionedInternalField().mesh()();
    const Time& t = mesh.time();

    vector u(vector::zero);
    vector a(vector::zero);
//    vector v(vector::zero);
//    double ang;
//    double tmpx[3], tmpy[3], tmpz[3];

    vectorList& disp = BD::disp();  // linear displacement
    vectorList& adisp = BD::adisp(); // angular displacement
    Info<< "- with linear displacement : " << disp << endl;
    Info<< "- with angular displacement : " << adisp << endl;

    //
    // --loop over all surface nodes
    //
    forAll(*this, ptI)
    {
        // get displacement from pre-calculated shape function
        u = vector::zero;
        a = vector::zero;
        for( int inode=0; inode<BD::nnodes; ++inode )
        {
            for( int i=0; i<3; ++i )
            {
                u.component(i) += 
                    BD::h()[ptI*BD::nnodes+inode] * disp[inode].component(i);
                a.component(i) += 
                    BD::h()[ptI*BD::nnodes+inode] * adisp[inode].component(i);
            }
        }

        this->operator[](ptI) = u;

/////////////////////////////////////////////////////////////////////
// TODO: general rotations, retrieve rotation matrix
// TODO: account for origin not at (0 0 0)
// x-rotation, quick
//        ang = a.component(0);
//        v = localPoints[ptI] - BD::origin;
//        tmpx[0] = v.component(0)                                                - localPoints[ptI].component(0);
//        tmpx[1] = v.component(1)*Foam::cos(ang) - v.component(2)*Foam::sin(ang) - localPoints[ptI].component(1);
//        tmpx[2] = v.component(1)*Foam::sin(ang) + v.component(2)*Foam::cos(ang) - localPoints[ptI].component(2);
//        this->operator[](ptI).component(0) += tmpx[0];
//        this->operator[](ptI).component(1) += tmpx[1];
//        this->operator[](ptI).component(2) += tmpx[2];
/////////////////////////////////////////////////////////////////////

        if(BD::twoD) this->operator[](ptI).component(BD::bladeDir) = 0.0;

    }

    fixedValuePointPatchField<vector>::updateCoeffs();
}


void beamDynInterfacePointPatchVectorField::write(Ostream& os) const
{
    pointPatchField<vector>::write(os);
    writeEntry("value", os);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

makePointPatchTypeField
(
    pointPatchVectorField,
    beamDynInterfacePointPatchVectorField
);

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
