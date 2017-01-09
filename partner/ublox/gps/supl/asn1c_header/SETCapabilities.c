/*
 * Generated by asn1c-0.9.22 (http://lionet.info/asn1c)
 * From ASN.1 module "SUPL-START"
 * 	found in "supl.asn"
 * 	`asn1c -gen-PER -fnative-types`
 */

#include "SETCapabilities.h"

static asn_TYPE_member_t asn_MBR_SETCapabilities_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct SETCapabilities, posTechnology),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PosTechnology,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"posTechnology"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SETCapabilities, prefMethod),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrefMethod,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"prefMethod"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SETCapabilities, posProtocol),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PosProtocol,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"posProtocol"
		},
};
static ber_tlv_tag_t asn_DEF_SETCapabilities_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_SETCapabilities_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* posTechnology at 116 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* prefMethod at 117 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 } /* posProtocol at 118 */
};
static asn_SEQUENCE_specifics_t asn_SPC_SETCapabilities_specs_1 = {
	sizeof(struct SETCapabilities),
	offsetof(struct SETCapabilities, _asn_ctx),
	asn_MAP_SETCapabilities_tag2el_1,
	3,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	2,	/* Start extensions */
	4	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_SETCapabilities = {
	"SETCapabilities",
	"SETCapabilities",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	SEQUENCE_decode_uper,
	SEQUENCE_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_SETCapabilities_tags_1,
	sizeof(asn_DEF_SETCapabilities_tags_1)
		/sizeof(asn_DEF_SETCapabilities_tags_1[0]), /* 1 */
	asn_DEF_SETCapabilities_tags_1,	/* Same as above */
	sizeof(asn_DEF_SETCapabilities_tags_1)
		/sizeof(asn_DEF_SETCapabilities_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_SETCapabilities_1,
	3,	/* Elements count */
	&asn_SPC_SETCapabilities_specs_1	/* Additional specs */
};

