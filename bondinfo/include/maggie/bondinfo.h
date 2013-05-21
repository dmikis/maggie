#ifndef _BI_BONDINFO_H_
#define _BI_BONDINFO_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef unsigned char BI_BOND;
typedef const char * BI_ATOM_SIG;

/**
 * Open given datebase with bonds information.
 *
 * @param db_filename Database file name.
 */
void bi_open_db(const char * db_filename);

/**
 * Predefined atom signatures
 */
#define BI_ATOM_CARBON_SP3        "C.3"
#define BI_ATOM_CARBON_SP2        "C.2"
#define BI_ATOM_CARBON_SP1        "C.1"
#define BI_ATOM_CARBON_AROMATIC   "C.ar"
#define BI_ATOM_NITROGEN_SP3      "N.3"
#define BI_ATOM_NITROGEN_SP2      "N.2"
#define BI_ATOM_NITROGEN_SP1      "N.1"
#define BI_ATOM_NITROGEN_AROMATIC "N.ar"
#define BI_ATOM_ANY               "*"

/**
 * Predefined bond types.
 */
#define BI_BOND_SINGLE   0x00
#define BI_BOND_DOUBLE   0x01
#define BI_BOND_TRIPLE   0x02
#define BI_BOND_AROMATIC 0x03
#define BI_BOND_DUMMY    0xFF

/**
 * Get length for bond.
 *
 * @param f_atom
 * @param s_atom
 * @param bond_type
 * @returns
 */
double bi_bond_length(BI_ATOM_SIG f_atom, BI_ATOM_SIG s_atom, BI_BOND bond_type);

/**
 * Get plain angle between two bonds.
 *
 * @param f_atom
 * @param s_atom
 * @param t_atom
 * @returns
 */
double bi_bonds_angle(BI_ATOM_SIG f_atom, BI_ATOM_SIG s_atom, BI_ATOM_SIG t_atom);

/**
 * Close opened database.
 */
void bi_close_db(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _BI_BONDINFO_H_
