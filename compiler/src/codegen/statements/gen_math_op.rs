use ast::OperatorType;
use codegen::values::NumValue;
use codegen::NodeContext;
use inkwell::types::IntType;
use inkwell::values::{PointerValue, VectorValue};
use inkwell::FloatPredicate;

pub fn gen_math_op_statement(
    op: &OperatorType,
    lhs: usize,
    rhs: usize,
    node: &mut NodeContext,
) -> PointerValue {
    let left_num = NumValue::new(node.get_statement(lhs));
    let right_num = NumValue::new(node.get_statement(rhs));
    let result_num = NumValue::new_undef(node.ctx.context, node.ctx.allocb);
    let left_form = left_num.get_form(node.ctx.b);
    result_num.set_form(node.ctx.b, &left_form);

    let left_vec = left_num.get_vec(node.ctx.b);
    let right_vec = right_num.get_vec(node.ctx.b);
    let result_vec = match op {
        OperatorType::Identity => left_vec,
        OperatorType::Add => node.ctx
            .b
            .build_float_add(left_vec, right_vec, "num.add.vec"),
        OperatorType::Subtract => node.ctx
            .b
            .build_float_sub(left_vec, right_vec, "num.sub.vec"),
        OperatorType::Multiply => node.ctx
            .b
            .build_float_mul(left_vec, right_vec, "num.mul.vec"),
        OperatorType::Divide => node.ctx
            .b
            .build_float_div(left_vec, right_vec, "num.divide.vec"),
        OperatorType::Modulo => node.ctx
            .b
            .build_float_rem(left_vec, right_vec, "num.mod.vec"),
        OperatorType::Power => unimplemented!(),
        OperatorType::BitwiseAnd => apply_int_op(
            node,
            left_vec,
            right_vec,
            node.ctx.context.i32_type(),
            &|lhs, rhs| node.ctx.b.build_and(lhs, rhs, "num.and.vec"),
        ),
        OperatorType::BitwiseOr => apply_int_op(
            node,
            left_vec,
            right_vec,
            node.ctx.context.i32_type(),
            &|lhs, rhs| node.ctx.b.build_or(lhs, rhs, "num.or.vec"),
        ),
        OperatorType::BitwiseXor => apply_int_op(
            node,
            left_vec,
            right_vec,
            node.ctx.context.i32_type(),
            &|lhs, rhs| node.ctx.b.build_xor(lhs, rhs, "num.xor.vec"),
        ),
        OperatorType::LogicalAnd => apply_int_op(
            node,
            left_vec,
            right_vec,
            node.ctx.context.bool_type(),
            &|lhs, rhs| node.ctx.b.build_and(lhs, rhs, "num.and.vec"),
        ),
        OperatorType::LogicalOr => apply_int_op(
            node,
            left_vec,
            right_vec,
            node.ctx.context.bool_type(),
            &|lhs, rhs| node.ctx.b.build_or(lhs, rhs, "num.or.vec"),
        ),
        OperatorType::LogicalEqual => int_to_float_vec(
            node,
            node.ctx.b.build_float_compare(
                FloatPredicate::OEQ,
                left_vec,
                right_vec,
                "num.vec.equal",
            ),
        ),
        OperatorType::LogicalNotEqual => int_to_float_vec(
            node,
            node.ctx.b.build_float_compare(
                FloatPredicate::ONE,
                left_vec,
                right_vec,
                "num.vec.notequal",
            ),
        ),
        OperatorType::LogicalGt => int_to_float_vec(
            node,
            node.ctx
                .b
                .build_float_compare(FloatPredicate::OGT, left_vec, right_vec, "num.vec.gt"),
        ),
        OperatorType::LogicalLt => int_to_float_vec(
            node,
            node.ctx
                .b
                .build_float_compare(FloatPredicate::OLT, left_vec, right_vec, "num.vec.lt"),
        ),
        OperatorType::LogicalGte => int_to_float_vec(
            node,
            node.ctx
                .b
                .build_float_compare(FloatPredicate::OGE, left_vec, right_vec, "num.vec.gte"),
        ),
        OperatorType::LogicalLte => int_to_float_vec(
            node,
            node.ctx
                .b
                .build_float_compare(FloatPredicate::OLE, left_vec, right_vec, "num.vec.lte"),
        ),
    };
    result_num.set_vec(node.ctx.b, &result_vec);

    result_num.val
}

fn apply_int_op(
    node: &NodeContext,
    lhs: VectorValue,
    rhs: VectorValue,
    num_type: IntType,
    op: &Fn(VectorValue, VectorValue) -> VectorValue,
) -> VectorValue {
    let left_int = float_vec_to_int(node, lhs, num_type);
    let right_int = float_vec_to_int(node, rhs, num_type);
    let result_int = op(left_int, right_int);
    int_to_float_vec(node, result_int)
}

fn float_vec_to_int(node: &NodeContext, vec: VectorValue, num_type: IntType) -> VectorValue {
    node.ctx
        .b
        .build_float_to_signed_int(vec, num_type.vec_type(2), "num.vec.int")
}

fn int_to_float_vec(node: &NodeContext, int: VectorValue) -> VectorValue {
    node.ctx.b.build_signed_int_to_float(
        int,
        node.ctx.context.f32_type().vec_type(2),
        "num.vec.float",
    )
}
