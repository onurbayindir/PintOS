// Code generated by protoc-gen-go. DO NOT EDIT.
// source: google/firestore/v1beta1/query.proto

package firestore

import (
	fmt "fmt"
	math "math"

	proto "github.com/golang/protobuf/proto"
	wrappers "github.com/golang/protobuf/ptypes/wrappers"
	_ "google.golang.org/genproto/googleapis/api/annotations"
)

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

// This is a compile-time assertion to ensure that this generated file
// is compatible with the proto package it is being compiled against.
// A compilation error at this line likely means your copy of the
// proto package needs to be updated.
const _ = proto.ProtoPackageIsVersion3 // please upgrade the proto package

// A sort direction.
type StructuredQuery_Direction int32

const (
	// Unspecified.
	StructuredQuery_DIRECTION_UNSPECIFIED StructuredQuery_Direction = 0
	// Ascending.
	StructuredQuery_ASCENDING StructuredQuery_Direction = 1
	// Descending.
	StructuredQuery_DESCENDING StructuredQuery_Direction = 2
)

var StructuredQuery_Direction_name = map[int32]string{
	0: "DIRECTION_UNSPECIFIED",
	1: "ASCENDING",
	2: "DESCENDING",
}

var StructuredQuery_Direction_value = map[string]int32{
	"DIRECTION_UNSPECIFIED": 0,
	"ASCENDING":             1,
	"DESCENDING":            2,
}

func (x StructuredQuery_Direction) String() string {
	return proto.EnumName(StructuredQuery_Direction_name, int32(x))
}

func (StructuredQuery_Direction) EnumDescriptor() ([]byte, []int) {
	return fileDescriptor_1ae4429ffd6f5a03, []int{0, 0}
}

// A composite filter operator.
type StructuredQuery_CompositeFilter_Operator int32

const (
	// Unspecified. This value must not be used.
	StructuredQuery_CompositeFilter_OPERATOR_UNSPECIFIED StructuredQuery_CompositeFilter_Operator = 0
	// The results are required to satisfy each of the combined filters.
	StructuredQuery_CompositeFilter_AND StructuredQuery_CompositeFilter_Operator = 1
)

var StructuredQuery_CompositeFilter_Operator_name = map[int32]string{
	0: "OPERATOR_UNSPECIFIED",
	1: "AND",
}

var StructuredQuery_CompositeFilter_Operator_value = map[string]int32{
	"OPERATOR_UNSPECIFIED": 0,
	"AND":                  1,
}

func (x StructuredQuery_CompositeFilter_Operator) String() string {
	return proto.EnumName(StructuredQuery_CompositeFilter_Operator_name, int32(x))
}

func (StructuredQuery_CompositeFilter_Operator) EnumDescriptor() ([]byte, []int) {
	return fileDescriptor_1ae4429ffd6f5a03, []int{0, 2, 0}
}

// A field filter operator.
type StructuredQuery_FieldFilter_Operator int32

const (
	// Unspecified. This value must not be used.
	StructuredQuery_FieldFilter_OPERATOR_UNSPECIFIED StructuredQuery_FieldFilter_Operator = 0
	// Less than. Requires that the field come first in `order_by`.
	StructuredQuery_FieldFilter_LESS_THAN StructuredQuery_FieldFilter_Operator = 1
	// Less than or equal. Requires that the field come first in `order_by`.
	StructuredQuery_FieldFilter_LESS_THAN_OR_EQUAL StructuredQuery_FieldFilter_Operator = 2
	// Greater than. Requires that the field come first in `order_by`.
	StructuredQuery_FieldFilter_GREATER_THAN StructuredQuery_FieldFilter_Operator = 3
	// Greater than or equal. Requires that the field come first in
	// `order_by`.
	StructuredQuery_FieldFilter_GREATER_THAN_OR_EQUAL StructuredQuery_FieldFilter_Operator = 4
	// Equal.
	StructuredQuery_FieldFilter_EQUAL StructuredQuery_FieldFilter_Operator = 5
	// Contains. Requires that the field is an array.
	StructuredQuery_FieldFilter_ARRAY_CONTAINS StructuredQuery_FieldFilter_Operator = 7
	// In. Requires that `value` is a non-empty ArrayValue with at most 10
	// values.
	StructuredQuery_FieldFilter_IN StructuredQuery_FieldFilter_Operator = 8
	// Contains any. Requires that the field is an array and
	// `value` is a non-empty ArrayValue with at most 10 values.
	StructuredQuery_FieldFilter_ARRAY_CONTAINS_ANY StructuredQuery_FieldFilter_Operator = 9
)

var StructuredQuery_FieldFilter_Operator_name = map[int32]string{
	0: "OPERATOR_UNSPECIFIED",
	1: "LESS_THAN",
	2: "LESS_THAN_OR_EQUAL",
	3: "GREATER_THAN",
	4: "GREATER_THAN_OR_EQUAL",
	5: "EQUAL",
	7: "ARRAY_CONTAINS",
	8: "IN",
	9: "ARRAY_CONTAINS_ANY",
}

var StructuredQuery_FieldFilter_Operator_value = map[string]int32{
	"OPERATOR_UNSPECIFIED":  0,
	"LESS_THAN":             1,
	"LESS_THAN_OR_EQUAL":    2,
	"GREATER_THAN":          3,
	"GREATER_THAN_OR_EQUAL": 4,
	"EQUAL":                 5,
	"ARRAY_CONTAINS":        7,
	"IN":                    8,
	"ARRAY_CONTAINS_ANY":    9,
}

func (x StructuredQuery_FieldFilter_Operator) String() string {
	return proto.EnumName(StructuredQuery_FieldFilter_Operator_name, int32(x))
}

func (StructuredQuery_FieldFilter_Operator) EnumDescriptor() ([]byte, []int) {
	return fileDescriptor_1ae4429ffd6f5a03, []int{0, 3, 0}
}

// A unary operator.
type StructuredQuery_UnaryFilter_Operator int32

const (
	// Unspecified. This value must not be used.
	StructuredQuery_UnaryFilter_OPERATOR_UNSPECIFIED StructuredQuery_UnaryFilter_Operator = 0
	// Test if a field is equal to NaN.
	StructuredQuery_UnaryFilter_IS_NAN StructuredQuery_UnaryFilter_Operator = 2
	// Test if an expression evaluates to Null.
	StructuredQuery_UnaryFilter_IS_NULL StructuredQuery_UnaryFilter_Operator = 3
)

var StructuredQuery_UnaryFilter_Operator_name = map[int32]string{
	0: "OPERATOR_UNSPECIFIED",
	2: "IS_NAN",
	3: "IS_NULL",
}

var StructuredQuery_UnaryFilter_Operator_value = map[string]int32{
	"OPERATOR_UNSPECIFIED": 0,
	"IS_NAN":               2,
	"IS_NULL":              3,
}

func (x StructuredQuery_UnaryFilter_Operator) String() string {
	return proto.EnumName(StructuredQuery_UnaryFilter_Operator_name, int32(x))
}

func (StructuredQuery_UnaryFilter_Operator) EnumDescriptor() ([]byte, []int) {
	return fileDescriptor_1ae4429ffd6f5a03, []int{0, 4, 0}
}

// A Firestore query.
type StructuredQuery struct {
	// The projection to return.
	Select *StructuredQuery_Projection `protobuf:"bytes,1,opt,name=select,proto3" json:"select,omitempty"`
	// The collections to query.
	From []*StructuredQuery_CollectionSelector `protobuf:"bytes,2,rep,name=from,proto3" json:"from,omitempty"`
	// The filter to apply.
	Where *StructuredQuery_Filter `protobuf:"bytes,3,opt,name=where,proto3" json:"where,omitempty"`
	// The order to apply to the query results.
	//
	// Firestore guarantees a stable ordering through the following rules:
	//
	//  * Any field required to appear in `order_by`, that is not already
	//    specified in `order_by`, is appended to the order in field name order
	//    by default.
	//  * If an order on `__name__` is not specified, it is appended by default.
	//
	// Fields are appended with the same sort direction as the last order
	// specified, or 'ASCENDING' if no order was specified. For example:
	//
	//  * `SELECT * FROM Foo ORDER BY A` becomes
	//    `SELECT * FROM Foo ORDER BY A, __name__`
	//  * `SELECT * FROM Foo ORDER BY A DESC` becomes
	//    `SELECT * FROM Foo ORDER BY A DESC, __name__ DESC`
	//  * `SELECT * FROM Foo WHERE A > 1` becomes
	//    `SELECT * FROM Foo WHERE A > 1 ORDER BY A, __name__`
	OrderBy []*StructuredQuery_Order `protobuf:"bytes,4,rep,name=order_by,json=orderBy,proto3" json:"order_by,omitempty"`
	// A starting point for the query results.
	StartAt *Cursor `protobuf:"bytes,7,opt,name=start_at,json=startAt,proto3" json:"start_at,omitempty"`
	// A end point for the query results.
	EndAt *Cursor `protobuf:"bytes,8,opt,name=end_at,json=endAt,proto3" json:"end_at,omitempty"`
	// The number of results to skip.
	//
	// Applies before limit, but after all other constraints. Must be >= 0 if
	// specified.
	Offset int32 `protobuf:"varint,6,opt,name=offset,proto3" json:"offset,omitempty"`
	// The maximum number of results to return.
	//
	// Applies after all other constraints.
	// Must be >= 0 if specified.
	Limit                *wrappers.Int32Value `protobuf:"bytes,5,opt,name=limit,proto3" json:"limit,omitempty"`
	XXX_NoUnkeyedLiteral struct{}             `json:"-"`
	XXX_unrecognized     []byte               `json:"-"`
	XXX_sizecache        int32                `json:"-"`
}

func (m *StructuredQuery) Reset()         { *m = StructuredQuery{} }
func (m *StructuredQuery) String() string { return proto.CompactTextString(m) }
func (*StructuredQuery) ProtoMessage()    {}
func (*StructuredQuery) Descriptor() ([]byte, []int) {
	return fileDescriptor_1ae4429ffd6f5a03, []int{0}
}

func (m *StructuredQuery) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_StructuredQuery.Unmarshal(m, b)
}
func (m *StructuredQuery) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_StructuredQuery.Marshal(b, m, deterministic)
}
func (m *StructuredQuery) XXX_Merge(src proto.Message) {
	xxx_messageInfo_StructuredQuery.Merge(m, src)
}
func (m *StructuredQuery) XXX_Size() int {
	return xxx_messageInfo_StructuredQuery.Size(m)
}
func (m *StructuredQuery) XXX_DiscardUnknown() {
	xxx_messageInfo_StructuredQuery.DiscardUnknown(m)
}

var xxx_messageInfo_StructuredQuery proto.InternalMessageInfo

func (m *StructuredQuery) GetSelect() *StructuredQuery_Projection {
	if m != nil {
		return m.Select
	}
	return nil
}

func (m *StructuredQuery) GetFrom() []*StructuredQuery_CollectionSelector {
	if m != nil {
		return m.From
	}
	return nil
}

func (m *StructuredQuery) GetWhere() *StructuredQuery_Filter {
	if m != nil {
		return m.Where
	}
	return nil
}

func (m *StructuredQuery) GetOrderBy() []*StructuredQuery_Order {
	if m != nil {
		return m.OrderBy
	}
	return nil
}

func (m *StructuredQuery) GetStartAt() *Cursor {
	if m != nil {
		return m.StartAt
	}
	return nil
}

func (m *StructuredQuery) GetEndAt() *Cursor {
	if m != nil {
		return m.EndAt
	}
	return nil
}

func (m *StructuredQuery) GetOffset() int32 {
	if m != nil {
		return m.Offset
	}
	return 0
}

func (m *StructuredQuery) GetLimit() *wrappers.Int32Value {
	if m != nil {
		return m.Limit
	}
	return nil
}

// A selection of a collection, such as `messages as m1`.
type StructuredQuery_CollectionSelector struct {
	// The collection ID.
	// When set, selects only collections with this ID.
	CollectionId string `protobuf:"bytes,2,opt,name=collection_id,json=collectionId,proto3" json:"collection_id,omitempty"`
	// When false, selects only collections that are immediate children of
	// the `parent` specified in the containing `RunQueryRequest`.
	// When true, selects all descendant collections.
	AllDescendants       bool     `protobuf:"varint,3,opt,name=all_descendants,json=allDescendants,proto3" json:"all_descendants,omitempty"`
	XXX_NoUnkeyedLiteral struct{} `json:"-"`
	XXX_unrecognized     []byte   `json:"-"`
	XXX_sizecache        int32    `json:"-"`
}

func (m *StructuredQuery_CollectionSelector) Reset()         { *m = StructuredQuery_CollectionSelector{} }
func (m *StructuredQuery_CollectionSelector) String() string { return proto.CompactTextString(m) }
func (*StructuredQuery_CollectionSelector) ProtoMessage()    {}
func (*StructuredQuery_CollectionSelector) Descriptor() ([]byte, []int) {
	return fileDescriptor_1ae4429ffd6f5a03, []int{0, 0}
}

func (m *StructuredQuery_CollectionSelector) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_StructuredQuery_CollectionSelector.Unmarshal(m, b)
}
func (m *StructuredQuery_CollectionSelector) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_StructuredQuery_CollectionSelector.Marshal(b, m, deterministic)
}
func (m *StructuredQuery_CollectionSelector) XXX_Merge(src proto.Message) {
	xxx_messageInfo_StructuredQuery_CollectionSelector.Merge(m, src)
}
func (m *StructuredQuery_CollectionSelector) XXX_Size() int {
	return xxx_messageInfo_StructuredQuery_CollectionSelector.Size(m)
}
func (m *StructuredQuery_CollectionSelector) XXX_DiscardUnknown() {
	xxx_messageInfo_StructuredQuery_CollectionSelector.DiscardUnknown(m)
}

var xxx_messageInfo_StructuredQuery_CollectionSelector proto.InternalMessageInfo

func (m *StructuredQuery_CollectionSelector) GetCollectionId() string {
	if m != nil {
		return m.CollectionId
	}
	return ""
}

func (m *StructuredQuery_CollectionSelector) GetAllDescendants() bool {
	if m != nil {
		return m.AllDescendants
	}
	return false
}

// A filter.
type StructuredQuery_Filter struct {
	// The type of filter.
	//
	// Types that are valid to be assigned to FilterType:
	//	*StructuredQuery_Filter_CompositeFilter
	//	*StructuredQuery_Filter_FieldFilter
	//	*StructuredQuery_Filter_UnaryFilter
	FilterType           isStructuredQuery_Filter_FilterType `protobuf_oneof:"filter_type"`
	XXX_NoUnkeyedLiteral struct{}                            `json:"-"`
	XXX_unrecognized     []byte                              `json:"-"`
	XXX_sizecache        int32                               `json:"-"`
}

func (m *StructuredQuery_Filter) Reset()         { *m = StructuredQuery_Filter{} }
func (m *StructuredQuery_Filter) String() string { return proto.CompactTextString(m) }
func (*StructuredQuery_Filter) ProtoMessage()    {}
func (*StructuredQuery_Filter) Descriptor() ([]byte, []int) {
	return fileDescriptor_1ae4429ffd6f5a03, []int{0, 1}
}

func (m *StructuredQuery_Filter) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_StructuredQuery_Filter.Unmarshal(m, b)
}
func (m *StructuredQuery_Filter) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_StructuredQuery_Filter.Marshal(b, m, deterministic)
}
func (m *StructuredQuery_Filter) XXX_Merge(src proto.Message) {
	xxx_messageInfo_StructuredQuery_Filter.Merge(m, src)
}
func (m *StructuredQuery_Filter) XXX_Size() int {
	return xxx_messageInfo_StructuredQuery_Filter.Size(m)
}
func (m *StructuredQuery_Filter) XXX_DiscardUnknown() {
	xxx_messageInfo_StructuredQuery_Filter.DiscardUnknown(m)
}

var xxx_messageInfo_StructuredQuery_Filter proto.InternalMessageInfo

type isStructuredQuery_Filter_FilterType interface {
	isStructuredQuery_Filter_FilterType()
}

type StructuredQuery_Filter_CompositeFilter struct {
	CompositeFilter *StructuredQuery_CompositeFilter `protobuf:"bytes,1,opt,name=composite_filter,json=compositeFilter,proto3,oneof"`
}

type StructuredQuery_Filter_FieldFilter struct {
	FieldFilter *StructuredQuery_FieldFilter `protobuf:"bytes,2,opt,name=field_filter,json=fieldFilter,proto3,oneof"`
}

type StructuredQuery_Filter_UnaryFilter struct {
	UnaryFilter *StructuredQuery_UnaryFilter `protobuf:"bytes,3,opt,name=unary_filter,json=unaryFilter,proto3,oneof"`
}

func (*StructuredQuery_Filter_CompositeFilter) isStructuredQuery_Filter_FilterType() {}

func (*StructuredQuery_Filter_FieldFilter) isStructuredQuery_Filter_FilterType() {}

func (*StructuredQuery_Filter_UnaryFilter) isStructuredQuery_Filter_FilterType() {}

func (m *StructuredQuery_Filter) GetFilterType() isStructuredQuery_Filter_FilterType {
	if m != nil {
		return m.FilterType
	}
	return nil
}

func (m *StructuredQuery_Filter) GetCompositeFilter() *StructuredQuery_CompositeFilter {
	if x, ok := m.GetFilterType().(*StructuredQuery_Filter_CompositeFilter); ok {
		return x.CompositeFilter
	}
	return nil
}

func (m *StructuredQuery_Filter) GetFieldFilter() *StructuredQuery_FieldFilter {
	if x, ok := m.GetFilterType().(*StructuredQuery_Filter_FieldFilter); ok {
		return x.FieldFilter
	}
	return nil
}

func (m *StructuredQuery_Filter) GetUnaryFilter() *StructuredQuery_UnaryFilter {
	if x, ok := m.GetFilterType().(*StructuredQuery_Filter_UnaryFilter); ok {
		return x.UnaryFilter
	}
	return nil
}

// XXX_OneofWrappers is for the internal use of the proto package.
func (*StructuredQuery_Filter) XXX_OneofWrappers() []interface{} {
	return []interface{}{
		(*StructuredQuery_Filter_CompositeFilter)(nil),
		(*StructuredQuery_Filter_FieldFilter)(nil),
		(*StructuredQuery_Filter_UnaryFilter)(nil),
	}
}

// A filter that merges multiple other filters using the given operator.
type StructuredQuery_CompositeFilter struct {
	// The operator for combining multiple filters.
	Op StructuredQuery_CompositeFilter_Operator `protobuf:"varint,1,opt,name=op,proto3,enum=google.firestore.v1beta1.StructuredQuery_CompositeFilter_Operator" json:"op,omitempty"`
	// The list of filters to combine.
	// Must contain at least one filter.
	Filters              []*StructuredQuery_Filter `protobuf:"bytes,2,rep,name=filters,proto3" json:"filters,omitempty"`
	XXX_NoUnkeyedLiteral struct{}                  `json:"-"`
	XXX_unrecognized     []byte                    `json:"-"`
	XXX_sizecache        int32                     `json:"-"`
}

func (m *StructuredQuery_CompositeFilter) Reset()         { *m = StructuredQuery_CompositeFilter{} }
func (m *StructuredQuery_CompositeFilter) String() string { return proto.CompactTextString(m) }
func (*StructuredQuery_CompositeFilter) ProtoMessage()    {}
func (*StructuredQuery_CompositeFilter) Descriptor() ([]byte, []int) {
	return fileDescriptor_1ae4429ffd6f5a03, []int{0, 2}
}

func (m *StructuredQuery_CompositeFilter) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_StructuredQuery_CompositeFilter.Unmarshal(m, b)
}
func (m *StructuredQuery_CompositeFilter) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_StructuredQuery_CompositeFilter.Marshal(b, m, deterministic)
}
func (m *StructuredQuery_CompositeFilter) XXX_Merge(src proto.Message) {
	xxx_messageInfo_StructuredQuery_CompositeFilter.Merge(m, src)
}
func (m *StructuredQuery_CompositeFilter) XXX_Size() int {
	return xxx_messageInfo_StructuredQuery_CompositeFilter.Size(m)
}
func (m *StructuredQuery_CompositeFilter) XXX_DiscardUnknown() {
	xxx_messageInfo_StructuredQuery_CompositeFilter.DiscardUnknown(m)
}

var xxx_messageInfo_StructuredQuery_CompositeFilter proto.InternalMessageInfo

func (m *StructuredQuery_CompositeFilter) GetOp() StructuredQuery_CompositeFilter_Operator {
	if m != nil {
		return m.Op
	}
	return StructuredQuery_CompositeFilter_OPERATOR_UNSPECIFIED
}

func (m *StructuredQuery_CompositeFilter) GetFilters() []*StructuredQuery_Filter {
	if m != nil {
		return m.Filters
	}
	return nil
}

// A filter on a specific field.
type StructuredQuery_FieldFilter struct {
	// The field to filter by.
	Field *StructuredQuery_FieldReference `protobuf:"bytes,1,opt,name=field,proto3" json:"field,omitempty"`
	// The operator to filter by.
	Op StructuredQuery_FieldFilter_Operator `protobuf:"varint,2,opt,name=op,proto3,enum=google.firestore.v1beta1.StructuredQuery_FieldFilter_Operator" json:"op,omitempty"`
	// The value to compare to.
	Value                *Value   `protobuf:"bytes,3,opt,name=value,proto3" json:"value,omitempty"`
	XXX_NoUnkeyedLiteral struct{} `json:"-"`
	XXX_unrecognized     []byte   `json:"-"`
	XXX_sizecache        int32    `json:"-"`
}

func (m *StructuredQuery_FieldFilter) Reset()         { *m = StructuredQuery_FieldFilter{} }
func (m *StructuredQuery_FieldFilter) String() string { return proto.CompactTextString(m) }
func (*StructuredQuery_FieldFilter) ProtoMessage()    {}
func (*StructuredQuery_FieldFilter) Descriptor() ([]byte, []int) {
	return fileDescriptor_1ae4429ffd6f5a03, []int{0, 3}
}

func (m *StructuredQuery_FieldFilter) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_StructuredQuery_FieldFilter.Unmarshal(m, b)
}
func (m *StructuredQuery_FieldFilter) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_StructuredQuery_FieldFilter.Marshal(b, m, deterministic)
}
func (m *StructuredQuery_FieldFilter) XXX_Merge(src proto.Message) {
	xxx_messageInfo_StructuredQuery_FieldFilter.Merge(m, src)
}
func (m *StructuredQuery_FieldFilter) XXX_Size() int {
	return xxx_messageInfo_StructuredQuery_FieldFilter.Size(m)
}
func (m *StructuredQuery_FieldFilter) XXX_DiscardUnknown() {
	xxx_messageInfo_StructuredQuery_FieldFilter.DiscardUnknown(m)
}

var xxx_messageInfo_StructuredQuery_FieldFilter proto.InternalMessageInfo

func (m *StructuredQuery_FieldFilter) GetField() *StructuredQuery_FieldReference {
	if m != nil {
		return m.Field
	}
	return nil
}

func (m *StructuredQuery_FieldFilter) GetOp() StructuredQuery_FieldFilter_Operator {
	if m != nil {
		return m.Op
	}
	return StructuredQuery_FieldFilter_OPERATOR_UNSPECIFIED
}

func (m *StructuredQuery_FieldFilter) GetValue() *Value {
	if m != nil {
		return m.Value
	}
	return nil
}

// A filter with a single operand.
type StructuredQuery_UnaryFilter struct {
	// The unary operator to apply.
	Op StructuredQuery_UnaryFilter_Operator `protobuf:"varint,1,opt,name=op,proto3,enum=google.firestore.v1beta1.StructuredQuery_UnaryFilter_Operator" json:"op,omitempty"`
	// The argument to the filter.
	//
	// Types that are valid to be assigned to OperandType:
	//	*StructuredQuery_UnaryFilter_Field
	OperandType          isStructuredQuery_UnaryFilter_OperandType `protobuf_oneof:"operand_type"`
	XXX_NoUnkeyedLiteral struct{}                                  `json:"-"`
	XXX_unrecognized     []byte                                    `json:"-"`
	XXX_sizecache        int32                                     `json:"-"`
}

func (m *StructuredQuery_UnaryFilter) Reset()         { *m = StructuredQuery_UnaryFilter{} }
func (m *StructuredQuery_UnaryFilter) String() string { return proto.CompactTextString(m) }
func (*StructuredQuery_UnaryFilter) ProtoMessage()    {}
func (*StructuredQuery_UnaryFilter) Descriptor() ([]byte, []int) {
	return fileDescriptor_1ae4429ffd6f5a03, []int{0, 4}
}

func (m *StructuredQuery_UnaryFilter) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_StructuredQuery_UnaryFilter.Unmarshal(m, b)
}
func (m *StructuredQuery_UnaryFilter) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_StructuredQuery_UnaryFilter.Marshal(b, m, deterministic)
}
func (m *StructuredQuery_UnaryFilter) XXX_Merge(src proto.Message) {
	xxx_messageInfo_StructuredQuery_UnaryFilter.Merge(m, src)
}
func (m *StructuredQuery_UnaryFilter) XXX_Size() int {
	return xxx_messageInfo_StructuredQuery_UnaryFilter.Size(m)
}
func (m *StructuredQuery_UnaryFilter) XXX_DiscardUnknown() {
	xxx_messageInfo_StructuredQuery_UnaryFilter.DiscardUnknown(m)
}

var xxx_messageInfo_StructuredQuery_UnaryFilter proto.InternalMessageInfo

func (m *StructuredQuery_UnaryFilter) GetOp() StructuredQuery_UnaryFilter_Operator {
	if m != nil {
		return m.Op
	}
	return StructuredQuery_UnaryFilter_OPERATOR_UNSPECIFIED
}

type isStructuredQuery_UnaryFilter_OperandType interface {
	isStructuredQuery_UnaryFilter_OperandType()
}

type StructuredQuery_UnaryFilter_Field struct {
	Field *StructuredQuery_FieldReference `protobuf:"bytes,2,opt,name=field,proto3,oneof"`
}

func (*StructuredQuery_UnaryFilter_Field) isStructuredQuery_UnaryFilter_OperandType() {}

func (m *StructuredQuery_UnaryFilter) GetOperandType() isStructuredQuery_UnaryFilter_OperandType {
	if m != nil {
		return m.OperandType
	}
	return nil
}

func (m *StructuredQuery_UnaryFilter) GetField() *StructuredQuery_FieldReference {
	if x, ok := m.GetOperandType().(*StructuredQuery_UnaryFilter_Field); ok {
		return x.Field
	}
	return nil
}

// XXX_OneofWrappers is for the internal use of the proto package.
func (*StructuredQuery_UnaryFilter) XXX_OneofWrappers() []interface{} {
	return []interface{}{
		(*StructuredQuery_UnaryFilter_Field)(nil),
	}
}

// An order on a field.
type StructuredQuery_Order struct {
	// The field to order by.
	Field *StructuredQuery_FieldReference `protobuf:"bytes,1,opt,name=field,proto3" json:"field,omitempty"`
	// The direction to order by. Defaults to `ASCENDING`.
	Direction            StructuredQuery_Direction `protobuf:"varint,2,opt,name=direction,proto3,enum=google.firestore.v1beta1.StructuredQuery_Direction" json:"direction,omitempty"`
	XXX_NoUnkeyedLiteral struct{}                  `json:"-"`
	XXX_unrecognized     []byte                    `json:"-"`
	XXX_sizecache        int32                     `json:"-"`
}

func (m *StructuredQuery_Order) Reset()         { *m = StructuredQuery_Order{} }
func (m *StructuredQuery_Order) String() string { return proto.CompactTextString(m) }
func (*StructuredQuery_Order) ProtoMessage()    {}
func (*StructuredQuery_Order) Descriptor() ([]byte, []int) {
	return fileDescriptor_1ae4429ffd6f5a03, []int{0, 5}
}

func (m *StructuredQuery_Order) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_StructuredQuery_Order.Unmarshal(m, b)
}
func (m *StructuredQuery_Order) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_StructuredQuery_Order.Marshal(b, m, deterministic)
}
func (m *StructuredQuery_Order) XXX_Merge(src proto.Message) {
	xxx_messageInfo_StructuredQuery_Order.Merge(m, src)
}
func (m *StructuredQuery_Order) XXX_Size() int {
	return xxx_messageInfo_StructuredQuery_Order.Size(m)
}
func (m *StructuredQuery_Order) XXX_DiscardUnknown() {
	xxx_messageInfo_StructuredQuery_Order.DiscardUnknown(m)
}

var xxx_messageInfo_StructuredQuery_Order proto.InternalMessageInfo

func (m *StructuredQuery_Order) GetField() *StructuredQuery_FieldReference {
	if m != nil {
		return m.Field
	}
	return nil
}

func (m *StructuredQuery_Order) GetDirection() StructuredQuery_Direction {
	if m != nil {
		return m.Direction
	}
	return StructuredQuery_DIRECTION_UNSPECIFIED
}

// The projection of document's fields to return.
type StructuredQuery_Projection struct {
	// The fields to return.
	//
	// If empty, all fields are returned. To only return the name
	// of the document, use `['__name__']`.
	Fields               []*StructuredQuery_FieldReference `protobuf:"bytes,2,rep,name=fields,proto3" json:"fields,omitempty"`
	XXX_NoUnkeyedLiteral struct{}                          `json:"-"`
	XXX_unrecognized     []byte                            `json:"-"`
	XXX_sizecache        int32                             `json:"-"`
}

func (m *StructuredQuery_Projection) Reset()         { *m = StructuredQuery_Projection{} }
func (m *StructuredQuery_Projection) String() string { return proto.CompactTextString(m) }
func (*StructuredQuery_Projection) ProtoMessage()    {}
func (*StructuredQuery_Projection) Descriptor() ([]byte, []int) {
	return fileDescriptor_1ae4429ffd6f5a03, []int{0, 6}
}

func (m *StructuredQuery_Projection) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_StructuredQuery_Projection.Unmarshal(m, b)
}
func (m *StructuredQuery_Projection) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_StructuredQuery_Projection.Marshal(b, m, deterministic)
}
func (m *StructuredQuery_Projection) XXX_Merge(src proto.Message) {
	xxx_messageInfo_StructuredQuery_Projection.Merge(m, src)
}
func (m *StructuredQuery_Projection) XXX_Size() int {
	return xxx_messageInfo_StructuredQuery_Projection.Size(m)
}
func (m *StructuredQuery_Projection) XXX_DiscardUnknown() {
	xxx_messageInfo_StructuredQuery_Projection.DiscardUnknown(m)
}

var xxx_messageInfo_StructuredQuery_Projection proto.InternalMessageInfo

func (m *StructuredQuery_Projection) GetFields() []*StructuredQuery_FieldReference {
	if m != nil {
		return m.Fields
	}
	return nil
}

// A reference to a field, such as `max(messages.time) as max_time`.
type StructuredQuery_FieldReference struct {
	FieldPath            string   `protobuf:"bytes,2,opt,name=field_path,json=fieldPath,proto3" json:"field_path,omitempty"`
	XXX_NoUnkeyedLiteral struct{} `json:"-"`
	XXX_unrecognized     []byte   `json:"-"`
	XXX_sizecache        int32    `json:"-"`
}

func (m *StructuredQuery_FieldReference) Reset()         { *m = StructuredQuery_FieldReference{} }
func (m *StructuredQuery_FieldReference) String() string { return proto.CompactTextString(m) }
func (*StructuredQuery_FieldReference) ProtoMessage()    {}
func (*StructuredQuery_FieldReference) Descriptor() ([]byte, []int) {
	return fileDescriptor_1ae4429ffd6f5a03, []int{0, 7}
}

func (m *StructuredQuery_FieldReference) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_StructuredQuery_FieldReference.Unmarshal(m, b)
}
func (m *StructuredQuery_FieldReference) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_StructuredQuery_FieldReference.Marshal(b, m, deterministic)
}
func (m *StructuredQuery_FieldReference) XXX_Merge(src proto.Message) {
	xxx_messageInfo_StructuredQuery_FieldReference.Merge(m, src)
}
func (m *StructuredQuery_FieldReference) XXX_Size() int {
	return xxx_messageInfo_StructuredQuery_FieldReference.Size(m)
}
func (m *StructuredQuery_FieldReference) XXX_DiscardUnknown() {
	xxx_messageInfo_StructuredQuery_FieldReference.DiscardUnknown(m)
}

var xxx_messageInfo_StructuredQuery_FieldReference proto.InternalMessageInfo

func (m *StructuredQuery_FieldReference) GetFieldPath() string {
	if m != nil {
		return m.FieldPath
	}
	return ""
}

// A position in a query result set.
type Cursor struct {
	// The values that represent a position, in the order they appear in
	// the order by clause of a query.
	//
	// Can contain fewer values than specified in the order by clause.
	Values []*Value `protobuf:"bytes,1,rep,name=values,proto3" json:"values,omitempty"`
	// If the position is just before or just after the given values, relative
	// to the sort order defined by the query.
	Before               bool     `protobuf:"varint,2,opt,name=before,proto3" json:"before,omitempty"`
	XXX_NoUnkeyedLiteral struct{} `json:"-"`
	XXX_unrecognized     []byte   `json:"-"`
	XXX_sizecache        int32    `json:"-"`
}

func (m *Cursor) Reset()         { *m = Cursor{} }
func (m *Cursor) String() string { return proto.CompactTextString(m) }
func (*Cursor) ProtoMessage()    {}
func (*Cursor) Descriptor() ([]byte, []int) {
	return fileDescriptor_1ae4429ffd6f5a03, []int{1}
}

func (m *Cursor) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_Cursor.Unmarshal(m, b)
}
func (m *Cursor) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_Cursor.Marshal(b, m, deterministic)
}
func (m *Cursor) XXX_Merge(src proto.Message) {
	xxx_messageInfo_Cursor.Merge(m, src)
}
func (m *Cursor) XXX_Size() int {
	return xxx_messageInfo_Cursor.Size(m)
}
func (m *Cursor) XXX_DiscardUnknown() {
	xxx_messageInfo_Cursor.DiscardUnknown(m)
}

var xxx_messageInfo_Cursor proto.InternalMessageInfo

func (m *Cursor) GetValues() []*Value {
	if m != nil {
		return m.Values
	}
	return nil
}

func (m *Cursor) GetBefore() bool {
	if m != nil {
		return m.Before
	}
	return false
}

func init() {
	proto.RegisterEnum("google.firestore.v1beta1.StructuredQuery_Direction", StructuredQuery_Direction_name, StructuredQuery_Direction_value)
	proto.RegisterEnum("google.firestore.v1beta1.StructuredQuery_CompositeFilter_Operator", StructuredQuery_CompositeFilter_Operator_name, StructuredQuery_CompositeFilter_Operator_value)
	proto.RegisterEnum("google.firestore.v1beta1.StructuredQuery_FieldFilter_Operator", StructuredQuery_FieldFilter_Operator_name, StructuredQuery_FieldFilter_Operator_value)
	proto.RegisterEnum("google.firestore.v1beta1.StructuredQuery_UnaryFilter_Operator", StructuredQuery_UnaryFilter_Operator_name, StructuredQuery_UnaryFilter_Operator_value)
	proto.RegisterType((*StructuredQuery)(nil), "google.firestore.v1beta1.StructuredQuery")
	proto.RegisterType((*StructuredQuery_CollectionSelector)(nil), "google.firestore.v1beta1.StructuredQuery.CollectionSelector")
	proto.RegisterType((*StructuredQuery_Filter)(nil), "google.firestore.v1beta1.StructuredQuery.Filter")
	proto.RegisterType((*StructuredQuery_CompositeFilter)(nil), "google.firestore.v1beta1.StructuredQuery.CompositeFilter")
	proto.RegisterType((*StructuredQuery_FieldFilter)(nil), "google.firestore.v1beta1.StructuredQuery.FieldFilter")
	proto.RegisterType((*StructuredQuery_UnaryFilter)(nil), "google.firestore.v1beta1.StructuredQuery.UnaryFilter")
	proto.RegisterType((*StructuredQuery_Order)(nil), "google.firestore.v1beta1.StructuredQuery.Order")
	proto.RegisterType((*StructuredQuery_Projection)(nil), "google.firestore.v1beta1.StructuredQuery.Projection")
	proto.RegisterType((*StructuredQuery_FieldReference)(nil), "google.firestore.v1beta1.StructuredQuery.FieldReference")
	proto.RegisterType((*Cursor)(nil), "google.firestore.v1beta1.Cursor")
}

func init() {
	proto.RegisterFile("google/firestore/v1beta1/query.proto", fileDescriptor_1ae4429ffd6f5a03)
}

var fileDescriptor_1ae4429ffd6f5a03 = []byte{
	// 998 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0xac, 0x96, 0x5f, 0x6f, 0xe3, 0x44,
	0x10, 0xc0, 0xcf, 0x4e, 0xf3, 0x6f, 0xd2, 0xa6, 0x66, 0x05, 0x27, 0x13, 0x8e, 0xa3, 0x0a, 0x48,
	0xd7, 0x17, 0x12, 0xda, 0x72, 0x02, 0x74, 0x80, 0xe4, 0x24, 0x6e, 0x9b, 0x53, 0xe5, 0xa4, 0x9b,
	0xb6, 0x52, 0x4f, 0x15, 0x96, 0x63, 0xaf, 0x53, 0x23, 0xd7, 0x6b, 0xd6, 0xeb, 0x3b, 0xf5, 0xcb,
	0xf0, 0xc0, 0xe3, 0x3d, 0xf0, 0x7a, 0x7c, 0x06, 0x3e, 0x05, 0x9f, 0x04, 0x21, 0xaf, 0xd7, 0x49,
	0x7b, 0x55, 0x45, 0x52, 0x78, 0xcb, 0xce, 0xce, 0xfc, 0x66, 0x3c, 0x7f, 0x76, 0x02, 0x5f, 0xcc,
	0x28, 0x9d, 0x85, 0xa4, 0xeb, 0x07, 0x8c, 0x24, 0x9c, 0x32, 0xd2, 0x7d, 0xbd, 0x33, 0x25, 0xdc,
	0xd9, 0xe9, 0xfe, 0x92, 0x12, 0x76, 0xdd, 0x89, 0x19, 0xe5, 0x14, 0xe9, 0xb9, 0x56, 0x67, 0xae,
	0xd5, 0x91, 0x5a, 0xad, 0x67, 0xf7, 0xda, 0x7b, 0xd4, 0x4d, 0xaf, 0x48, 0xc4, 0x73, 0x44, 0xeb,
	0xa9, 0x54, 0x14, 0xa7, 0x69, 0xea, 0x77, 0xdf, 0x30, 0x27, 0x8e, 0x09, 0x4b, 0xe4, 0xfd, 0x13,
	0x79, 0xef, 0xc4, 0x41, 0xd7, 0x89, 0x22, 0xca, 0x1d, 0x1e, 0xd0, 0x48, 0xde, 0xb6, 0x7f, 0xff,
	0x00, 0x36, 0x27, 0x9c, 0xa5, 0x2e, 0x4f, 0x19, 0xf1, 0x8e, 0xb3, 0xd0, 0xd0, 0x11, 0x54, 0x12,
	0x12, 0x12, 0x97, 0xeb, 0xca, 0x96, 0xb2, 0xdd, 0xd8, 0xfd, 0xba, 0x73, 0x5f, 0x94, 0x9d, 0xf7,
	0x4c, 0x3b, 0x63, 0x46, 0x7f, 0x26, 0x6e, 0xe6, 0x00, 0x4b, 0x06, 0x1a, 0xc3, 0x9a, 0xcf, 0xe8,
	0x95, 0xae, 0x6e, 0x95, 0xb6, 0x1b, 0xbb, 0xdf, 0x2f, 0xcf, 0xea, 0xd3, 0x30, 0xcc, 0x59, 0x13,
	0x41, 0xa2, 0x0c, 0x0b, 0x12, 0xda, 0x87, 0xf2, 0x9b, 0x4b, 0xc2, 0x88, 0x5e, 0x12, 0xe1, 0x7d,
	0xb5, 0x3c, 0x72, 0x3f, 0x08, 0x39, 0x61, 0x38, 0x37, 0x47, 0x2f, 0xa1, 0x46, 0x99, 0x47, 0x98,
	0x3d, 0xbd, 0xd6, 0xd7, 0x44, 0x74, 0xdd, 0xe5, 0x51, 0xa3, 0xcc, 0x12, 0x57, 0x05, 0xa0, 0x77,
	0x8d, 0x5e, 0x40, 0x2d, 0xe1, 0x0e, 0xe3, 0xb6, 0xc3, 0xf5, 0xaa, 0x08, 0x6b, 0xeb, 0x7e, 0x56,
	0x3f, 0x65, 0x09, 0x65, 0xb8, 0x2a, 0x2c, 0x0c, 0x8e, 0xbe, 0x81, 0x0a, 0x89, 0xbc, 0xcc, 0xb4,
	0xb6, 0xa4, 0x69, 0x99, 0x44, 0x9e, 0xc1, 0xd1, 0x63, 0xa8, 0x50, 0xdf, 0x4f, 0x08, 0xd7, 0x2b,
	0x5b, 0xca, 0x76, 0x19, 0xcb, 0x13, 0xda, 0x81, 0x72, 0x18, 0x5c, 0x05, 0x5c, 0x2f, 0x0b, 0xde,
	0x27, 0x05, 0xaf, 0xe8, 0x91, 0xce, 0x30, 0xe2, 0x7b, 0xbb, 0x67, 0x4e, 0x98, 0x12, 0x9c, 0x6b,
	0xb6, 0xa6, 0x80, 0xee, 0x26, 0x1c, 0x7d, 0x0e, 0x1b, 0xee, 0x5c, 0x6a, 0x07, 0x9e, 0xae, 0x6e,
	0x29, 0xdb, 0x75, 0xbc, 0xbe, 0x10, 0x0e, 0x3d, 0xf4, 0x0c, 0x36, 0x9d, 0x30, 0xb4, 0x3d, 0x92,
	0xb8, 0x24, 0xf2, 0x9c, 0x88, 0x27, 0xa2, 0x32, 0x35, 0xdc, 0x74, 0xc2, 0x70, 0xb0, 0x90, 0xb6,
	0xde, 0xa9, 0x50, 0xc9, 0x4b, 0x80, 0x7c, 0xd0, 0x5c, 0x7a, 0x15, 0xd3, 0x24, 0xe0, 0xc4, 0xf6,
	0x85, 0x4c, 0x76, 0xdb, 0x77, 0xab, 0x74, 0x88, 0x24, 0xe4, 0xd0, 0xc3, 0x47, 0x78, 0xd3, 0xbd,
	0x2d, 0x42, 0xaf, 0x60, 0xdd, 0x0f, 0x48, 0xe8, 0x15, 0x3e, 0x54, 0xe1, 0xe3, 0xf9, 0x2a, 0x2d,
	0x43, 0x42, 0x6f, 0xce, 0x6f, 0xf8, 0x8b, 0x63, 0xc6, 0x4e, 0x23, 0x87, 0x5d, 0x17, 0xec, 0xd2,
	0xaa, 0xec, 0xd3, 0xcc, 0x7a, 0xc1, 0x4e, 0x17, 0xc7, 0xde, 0x06, 0x34, 0x72, 0xaa, 0xcd, 0xaf,
	0x63, 0xd2, 0xfa, 0x4b, 0x81, 0xcd, 0xf7, 0xbe, 0x16, 0x61, 0x50, 0x69, 0x2c, 0x92, 0xd6, 0xdc,
	0xed, 0x3d, 0x38, 0x69, 0x9d, 0x51, 0x4c, 0x98, 0x93, 0x0d, 0x97, 0x4a, 0x63, 0xf4, 0x12, 0xaa,
	0xb9, 0xdb, 0x44, 0xce, 0xeb, 0xea, 0xc3, 0x55, 0x00, 0xda, 0x5f, 0x42, 0xad, 0x60, 0x23, 0x1d,
	0x3e, 0x1c, 0x8d, 0x4d, 0x6c, 0x9c, 0x8c, 0xb0, 0x7d, 0x6a, 0x4d, 0xc6, 0x66, 0x7f, 0xb8, 0x3f,
	0x34, 0x07, 0xda, 0x23, 0x54, 0x85, 0x92, 0x61, 0x0d, 0x34, 0xa5, 0xf5, 0x6b, 0x09, 0x1a, 0x37,
	0x92, 0x8d, 0x2c, 0x28, 0x8b, 0x64, 0xcb, 0xb6, 0xf8, 0x76, 0xc5, 0x92, 0x61, 0xe2, 0x13, 0x46,
	0x22, 0x97, 0xe0, 0x1c, 0x83, 0x2c, 0x91, 0x2e, 0x55, 0xa4, 0xeb, 0xc7, 0x07, 0xd5, 0xff, 0x76,
	0xaa, 0x9e, 0x43, 0xf9, 0x75, 0x36, 0x40, 0xb2, 0xec, 0x9f, 0xdd, 0x8f, 0x94, 0x73, 0x26, 0xb4,
	0xdb, 0xef, 0x94, 0xa5, 0xd2, 0xb2, 0x01, 0xf5, 0x23, 0x73, 0x32, 0xb1, 0x4f, 0x0e, 0x0d, 0x4b,
	0x53, 0xd0, 0x63, 0x40, 0xf3, 0xa3, 0x3d, 0xc2, 0xb6, 0x79, 0x7c, 0x6a, 0x1c, 0x69, 0x2a, 0xd2,
	0x60, 0xfd, 0x00, 0x9b, 0xc6, 0x89, 0x89, 0x73, 0xcd, 0x12, 0xfa, 0x18, 0x3e, 0xba, 0x29, 0x59,
	0x28, 0xaf, 0xa1, 0x3a, 0x94, 0xf3, 0x9f, 0x65, 0x84, 0xa0, 0x69, 0x60, 0x6c, 0x9c, 0xdb, 0xfd,
	0x91, 0x75, 0x62, 0x0c, 0xad, 0x89, 0x56, 0x45, 0x15, 0x50, 0x87, 0x96, 0x56, 0xcb, 0x7c, 0xdd,
	0xbe, 0xb3, 0x0d, 0xeb, 0x5c, 0xab, 0xb7, 0xfe, 0x56, 0xa0, 0x71, 0xa3, 0x63, 0x65, 0x42, 0x95,
	0x55, 0x13, 0x7a, 0x03, 0x71, 0x3b, 0xa1, 0xe3, 0xa2, 0xe0, 0xea, 0x7f, 0x2b, 0xf8, 0xe1, 0x23,
	0x59, 0xf2, 0xf6, 0x0f, 0x4b, 0xa5, 0x1a, 0xa0, 0x32, 0x9c, 0xd8, 0x96, 0x61, 0x69, 0x2a, 0x6a,
	0x40, 0x35, 0xfb, 0x7d, 0x7a, 0x74, 0xa4, 0x95, 0x7a, 0x4d, 0x58, 0xa7, 0x99, 0x79, 0xe4, 0xe5,
	0x43, 0xf8, 0x56, 0x81, 0xb2, 0x78, 0xf6, 0xff, 0xf7, 0xde, 0x3c, 0x86, 0xba, 0x17, 0xb0, 0xfc,
	0x41, 0x95, 0x2d, 0xba, 0xb7, 0x3c, 0x73, 0x50, 0x98, 0xe2, 0x05, 0xa5, 0xf5, 0x13, 0xc0, 0x62,
	0x19, 0xa3, 0x31, 0x54, 0x84, 0xa7, 0x62, 0xac, 0x1f, 0x1e, 0xb1, 0xe4, 0xb4, 0xba, 0xd0, 0xbc,
	0x7d, 0x83, 0x3e, 0x05, 0xc8, 0x9f, 0xda, 0xd8, 0xe1, 0x97, 0x72, 0x51, 0xd4, 0x85, 0x64, 0xec,
	0xf0, 0xcb, 0xb6, 0x09, 0xf5, 0x79, 0xa0, 0x59, 0x97, 0x0e, 0x86, 0xd8, 0xec, 0x9f, 0x0c, 0x47,
	0xd6, 0xdd, 0xce, 0x37, 0x26, 0x7d, 0xd3, 0x1a, 0x0c, 0xad, 0x03, 0x4d, 0x41, 0x4d, 0x80, 0x81,
	0x39, 0x3f, 0xab, 0xed, 0x73, 0xa8, 0xe4, 0x3b, 0x30, 0xdb, 0x9a, 0x62, 0xa4, 0x12, 0x5d, 0x11,
	0xdf, 0xf4, 0xaf, 0x13, 0x28, 0xd5, 0xb3, 0xad, 0x39, 0x25, 0x3e, 0x65, 0x44, 0x04, 0x59, 0xc3,
	0xf2, 0xd4, 0xfb, 0x43, 0x81, 0x27, 0x2e, 0xbd, 0xba, 0x17, 0xd3, 0x03, 0x91, 0x91, 0x71, 0xb6,
	0x44, 0xc7, 0xca, 0x2b, 0x43, 0xea, 0xcd, 0x68, 0xe8, 0x44, 0xb3, 0x0e, 0x65, 0xb3, 0xee, 0x8c,
	0x44, 0x62, 0xc5, 0x76, 0xf3, 0x2b, 0x27, 0x0e, 0x92, 0xbb, 0x7f, 0xe0, 0x5e, 0xcc, 0x25, 0xbf,
	0xa9, 0x6b, 0x07, 0xfd, 0xfd, 0xc9, 0x5b, 0xf5, 0xe9, 0x41, 0x8e, 0xea, 0x87, 0x34, 0xf5, 0x3a,
	0xfb, 0x73, 0xc7, 0x67, 0x3b, 0xbd, 0xcc, 0xe2, 0xcf, 0x42, 0xe1, 0x42, 0x28, 0x5c, 0xcc, 0x15,
	0x2e, 0xce, 0x72, 0xe4, 0xb4, 0x22, 0xdc, 0xee, 0xfd, 0x13, 0x00, 0x00, 0xff, 0xff, 0xdc, 0x48,
	0xdf, 0x7e, 0x76, 0x0a, 0x00, 0x00,
}
