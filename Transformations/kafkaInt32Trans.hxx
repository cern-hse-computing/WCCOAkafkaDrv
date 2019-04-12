/** © Copyright 2019 CERN
 *
 * This software is distributed under the terms of the
 * GNU Lesser General Public Licence version 3 (LGPL Version 3),
 * copied verbatim in the file “LICENSE”
 *
 * In applying this licence, CERN does not waive the privileges
 * and immunities granted to it by virtue of its status as an
 * Intergovernmental Organization or submit itself to any jurisdiction.
 *
 * Author: Alexandru Savulescu (HSE-CEN-CO)
 *
 **/

#ifndef kafkaINT32TRANS_HXX_
#define kafkaINT32TRANS_HXX_

#include <Transformation.hxx>
namespace Transformations {


class kafkaInt32Trans: public Transformation {
	/*!
	 *  Transformations typ
	 *  \return transformation type
	 */
	TransformationType isA() const;
	/*!
	 *  Transformations typ comparison
	 *  \param type object to return type
	 *  \return transformation type
	 */
	TransformationType isA(TransformationType type) const;

	/*!
	 * Size of transformation buffer
	 * \return size of buffer
	 */
	int itemSize() const;

	/*!
	 * The type of Variable we are expecting here
	 * \return actual variable type
	 */
	VariableType getVariableType() const;

	/*!
	 *  Clone of our class
	 *  \return pointer to new object
	 */
	Transformation *clone() const;

	/*!
	 * Conversion from PVSS to Hardware
	 * \param dataPtr pointer to buffer where data will be written
	 * \param len size of data buffer
	 * \param var reference to current translated value
	 * \param subix subindex of value in data point
	 * \return flag if translation was successful
	 */
	PVSSboolean toPeriph(PVSSchar *dataPtr, PVSSuint len, const Variable &var,
			const PVSSuint subix) const;

	/*!
	 * Conversion from Hardware to PVSS
	 * \param data pointer to buffer from where data will be read
	 * \param dlen length of data buffer
	 * \param subix subindex of value associated with peripheral address
	 * \return flag if translation was successful
	 */
	VariablePtr toVar(const PVSSchar *data, const PVSSuint dlen,
			const PVSSuint subix) const;

private:
	const static uint8_t size = sizeof(int32_t);
};

}

#endif /* kafkaINT32TRANS_HXX_ */

